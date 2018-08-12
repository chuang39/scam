#include "scam.hpp"


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
        pool.end_at = now() + 24 * 3600;
        pool.key_balance = 0;
        pool.eos_balance = asset(0, symbol_type(S(4, EOS)));
        pool.key_price = asset(1000, symbol_type(S(4, EOS)));
        pool.eos_total = asset(0, symbol_type(S(4, EOS)));
    });
    for( const auto& pool : pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner);
    }
}

void scam::checkpool() {
    auto pool = pools.begin();

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
        pools.emplace(owner, [&](auto &pool) {
            pool.id = pools.available_primary_key();
            pool.poolname = string(poolname);
            pool.owner = _self;
            pool.lastbuyer = _self;
            pool.status = 1;
            pool.round = next_round;
            pool.created_at = now();
            pool.end_at = now() + 24 * 3600;
            pool.key_balance = 10;
            pool.eos_balance = asset(0, symbol_type(S(4, EOS)));
            pool.key_price = asset(1000, symbol_type(S(4, EOS)));
            pool.eos_total = asset(0, symbol_type(S(4, EOS)));
        });
    }
}


void scam::deposit(const currency::transfer &t, account_name code) {
    if(from == _self) {
        return;
    }

    checkpool();

    /*
    print("\n>>> sender >>>", from, " - name: ", name{from});

    eosio_assert(quantity.symbol == string_to_symbol(4, "EOS"), "Only accepts EOS for deposits");
    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "Quantity must be positive");

    // find account
    auto itr_acnt = accounts.find(from);
    if(itr_acnt != accounts.end()) {
        accounts.modify(itr_acnt, _self, [&](auto& r){
            // Assumption: total currency issued by eosio.token will not overflow asset
            r.balance += transfer_data.quantity;
            new_balance = r.balance;
        });
    } else {
        accounts.emplace(_self, [&](auto& r){
            r.balance = transfer_data.quantity;
            new_balance = r.balance;
        });
    }
     */
}

void scam::withdraw(const account_name to) {
    require_auth(to);


/*

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(code == N(eosio.token);
    eosio_assert(quantity.amount > 0, "must withdraw positive quantity");

    auto itr = accounts.find(to);
    eosio_assert(itr != accounts.end(), "unknown account");

    accounts.modify( itr, 0, [&]( auto& acnt ) {
        eosio_assert( acnt.eos_balance >= quantity, "insufficient balance" );
        acnt.eos_balance -= quantity;
    });


    if( itr->is_empty() ) {
        accounts.erase(itr);
    }
    */
}

void scam::runwithdraw(const account_name to) {
    print(">>> withdraw:", name{to});

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
