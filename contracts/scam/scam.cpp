#include "scam.hpp"


uint64_t pricemap[8][2] = {{100000 * 16, 100},
                                {100000 * 32, 200},
                                {100000 * 64, 400},
                                {100000 * 128, 800},
                                {100000 * 256, 1600},
                                {100000 * 512, 3200},
                                {100000 * 1024, 6400},
                                {100000 * 2048, 12800}};

uint64_t get_price(uint64_t sold_keys) {
    for (int i = 0; i < 8; i++) {
        if (sold_keys < pricemap[i][0]) {
            return pricemap[i][1];
        }
    }
    return 25600;
}


void scam::createpool(const name owner, const string poolname) {
    require_auth(_self);

    // TODO: should only allow one active pool
    // TODO: add a method to control pool state
    print( "Create pool ", poolname, " by owner= ", name{owner} );

    pools.emplace(owner, [&](auto &pool) {
        pool.id = pools.available_primary_key();
        pool.poolname = string(poolname);
        pool.owner = name{owner};
        pool.lastbuyer = name{owner};
        pool.status = 1;
        pool.round = 1;
        pool.created_at = now();
        //pool.end_at = now() + 24 * 3600;
        pool.end_at = now() + 30;
        pool.last_buy_ts = now();
        pool.key_balance = 0;
        pool.eos_balance = 0;
        pool.key_price = 1000;
        pool.eos_total = 0;
    });
    for( const auto& pool : pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner);
    }
}

void scam::checkpool() {
    auto pool = pools.begin();
    if (pool == pools.end()) {
        print(">>> no pool is found");
        return;
    } else {
        print(">>> current time: ", now());
        print(">>> found the poolname: ", pool->poolname);
        print(">>> found the round: ", pool->round);
        print(">>> found the onwer: ", pool->owner);
        print(">>> found the created_at: ", pool->created_at);
        print(">>> found the end_at: ", pool->end_at);
        print(">>> found the key_balance: ", pool->key_balance);
        print(">>> found the eos_balance: ", pool->eos_balance);
    }

    if (pool->end_at <= now()) {
        auto winner = pool->lastbuyer;
        auto itr_winner = accounts.find(winner);
        if (itr_winner != accounts.end()) {
            accounts.modify(itr_winner, _self, [&](auto &p){
                p.eos_balance += pool->eos_balance;
            });
        }

        // get value from last pool to create next pool
        uint32_t next_round = pool->round + 1;
        string poolname = pool->poolname;

        // erase table
        auto itr = pools.begin();
        while (itr != pools.end()) {
            itr = pools.erase(itr);
        }

        // start new round
        pools.emplace(_self, [&](auto &p) {
            p.id = pools.available_primary_key();
            p.poolname = string(poolname);
            p.owner = name{_self};
            p.lastbuyer = name{_self};
            p.status = 1;
            p.round = next_round;
            p.created_at = now();
            //p.end_at = now() + 24 * 3600;
            p.end_at = now() + 60;
            p.last_buy_ts = now();
            p.key_balance = 0;
            p.eos_balance = 0;
            p.key_price = 1000;
            p.eos_total = 0;
        });
    }
}


void scam::deposit(const currency::transfer &t, account_name code) {
    if(code == _self) {
        return;
    }

    checkpool();

    eosio_assert(code == N(eosio.token), "Transfer not from eosio.token");
    eosio_assert(t.to == _self, "Transfer not made to this contract");
    eosio_assert(t.quantity.symbol == string_to_symbol(4, "EOS"), "Only accepts EOS for deposits");
    eosio_assert(t.quantity.is_valid(), "Invalid token transfer");
    eosio_assert(t.quantity.amount > 0, "Quantity must be positive");

    // find pool
    auto pool = pools.begin();
    if (pool == pools.end()) {
        print(">>> no pool is found");
        return;
    }

    auto user = t.from;
    auto amount = t.quantity.amount;
    uint64_t keybal = pool->key_balance;
    uint64_t cur_price = get_price(keybal);
    uint64_t keycnt = amount / cur_price;
    uint64_t new_price = get_price(keybal + keycnt);

    // pay dividend
    uint64_t dividend = accounts.begin() == accounts.end() ? 0 : (amount * DIVIDEND_PERCENT);
    for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
        auto share = dividend * ((double)itr->key_balance / (double)keybal);
        accounts.modify(itr, _self, [&](auto &p){
            p.eos_balance += share;
        });
    }

    // TODO: pay referral. Need to change team dividend too
    //uint64_t ref_bonus = amount * REFERRAL_PERCENT;
    uint64_t ref_bonus = 0;


    // add or update user
    auto itr_user = accounts.find(user);
    if (itr_user == accounts.end()) {
        itr_user = accounts.emplace(_self, [&](auto &p){
            p.owner = name{user};
            p.key_balance = 0;
            p.eos_balance = 0;
        });
    }
    accounts.modify(itr_user, _self, [&](auto &p){
        p.key_balance += keycnt;
    });

    // update pool
    uint64_t prize_share = amount * FINAL_PRIZE_PERCENT;
    pools.modify(pool, _self,  [&](auto &p) {
        p.lastbuyer = name{user};
        p.last_buy_ts = now();
        p.end_at = std::min(p.end_at + TIME_INC, p.last_buy_ts + DAY_IN_SEC);

        p.key_balance += keycnt;
        p.eos_balance += prize_share;
        p.eos_total += amount;
        if (p.key_price != new_price) {
            p.key_price = new_price;
        }
    });

    // pay team
    uint64_t team_share = amount - dividend - ref_bonus - prize_share;
    auto team = accounts.find(_self);
    if (team == accounts.end()) {
        team = accounts.emplace(_self, [&](auto &p) {
            p.owner = name{_self};
            p.key_balance = 0;
            p.eos_balance = 0;
        });
    }
    accounts.modify(team, _self, [&](auto &p) {
       p.eos_balance += team_share;
    });
}

void scam::withdraw(const account_name to) {
    require_auth(to);
}

void scam::runwithdraw(const account_name to) {
    print(">>> withdraw:", name{to});

    // find user
    auto itr = accounts.find(name{to});
    eosio_assert(itr != accounts.end(), "user not found");

    // clear balance
    asset bal = asset(itr->eos_balance, symbol_type(S(4, EOS)));
    accounts.modify(itr, _self, [&](auto &p) { p.eos_balance = 0; });

    // withdraw
    action(permission_level{_self, N(active)}, N(eosio.token),
           N(transfer), std::make_tuple(_self, to, bal,
                                        std::string("Money exit from EosScam")))
            .send();
}

//@abi action
void scam::deleteall() {
    require_auth(_self);

    auto itr = pools.begin();
    while (itr != pools.end()) {
        itr = pools.erase(itr);
    }

    auto itr_acnt = accounts.begin();
    while (itr_acnt != accounts.end()) {
        itr_acnt = accounts.erase(itr_acnt);
    }
}

void scam::reset() {
    require_auth(_self);

}

void scam::apply(account_name contract, account_name act) {
    print(">>> apply:", contract, ">>> act:", act);
    if (act == N(transfer)) {
        deposit(unpack_action_data<currency::transfer>(), contract);
        return;
    }

    if (act == N(withdraw)) {
        runwithdraw(contract);
        return;
    }

    if (contract != _self) {
        return;
    }

    auto &thiscontract = *this;
    switch (act) { EOSIO_API(scam, (withdraw)(createpool)(deleteall)(reset)); };
}

#define EOSIO_ABI_EX( TYPE, MEMBERS ) \
extern "C" { \
   void apply(uint64_t receiver, uint64_t code, uint64_t action) { \
      auto self = receiver; \
      if( action == N(onerror)) { \
         /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s "active permission */ \
         eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account"); \
      } \
      if(code == self || code == N(eosio.token) || action == N(onerror)) { \
         TYPE thiscontract(self); \
         if (action == N(transfer)) { \
             thiscontract.deposit(unpack_action_data<currency::transfer>(), code); \
             return; \
         } \
         if (action == N(withdraw)) { \
            thiscontract.runwithdraw(code); \
            return; \
         } \
         if (code != self) { \
             return; \
         } \
         switch(action) { \
            EOSIO_API(TYPE, MEMBERS) \
         } \
      } \
   } \
}

EOSIO_ABI_EX(scam, (withdraw)(checkpool)(createpool)(deleteall)(reset))
