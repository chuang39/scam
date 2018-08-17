#include "scam.hpp"


uint64_t pricemap[8][2] = {{100000 * 16, 100},
                                {100000 * 48, 162},
                                {100000 * 96, 262},
                                {100000 * 192, 424},
                                {100000 * 384, 685},
                                {100000 * 768, 1109},
                                {100000 * 1536, 1794},
                                {100000 * 3072, 2903}};
// TODO: add exception for overflow


uint64_t get_price(uint64_t sold_keys) {
    for (int i = 0; i < 8; i++) {
        if (sold_keys < pricemap[i][0]) {
            return pricemap[i][1];
        }
    }
    return 4697;
}

uint64_t get_level_keys(uint64_t sold_keys) {
    for (int i = 0; i < 8; i++) {
        if (sold_keys < pricemap[i][0]) {
            return pricemap[i][0] / 2;
        }
    }
    return pricemap[7][0] / 2;
}


void scam::createpool(const name owner, const string poolname) {
    require_auth(_self);

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
        //pool.end_at = now() + 30;
        pool.last_buy_ts = now();
        pool.key_balance = 0;
        pool.eos_balance = 0;
        pool.key_price = pricemap[0][1];
        pool.eos_total = 0;
        pool.dividend_paid = 0;
        pool.bonus_balance = 0;
        pool.bonus_keys_needed = pricemap[0][0];
        pool.total_time_in_sec = 0;
    });
    for( const auto& pool : pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner);
    }
}

void scam::ping() {
    require_auth(_self);
    checkpool();
}

void scam::pong() {
    print("hihi Kevin");
}

void scam::pong2(const name to) {
    require_auth(to);
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

    //update final table
    uint64_t finaltable_size = pool->key_balance * FINAL_TABLE_PORTION;
    uint64_t dump_size = pool->key_balance - finaltable_size;

    auto itr_ft = finaltable.begin();
    while (itr_ft != finaltable.end()) {
        if (itr_ft->end <= dump_size) {
            auto itr_fter = accounts.find(itr_ft->owner);
            if (itr_fter != accounts.end()) {
                uint64_t reduced_keys = itr_ft->end - itr_ft->start + 1;
                accounts.modify(itr_fter, _self, [&](auto &p){
                    p.finaltable_keys -= reduced_keys;
                });
            }
            itr_ft = finaltable.erase(itr_ft);
        } else if (itr_ft->start <= dump_size) {
            auto itr_fter = accounts.find(itr_ft->owner);
            if (itr_fter != accounts.end()) {
                accounts.modify(itr_fter, _self, [&](auto &p){
                    p.finaltable_keys -= (dump_size - itr_ft->start + 1);
                });
                finaltable.modify(itr_ft, _self, [&](auto &p){
                    p.start = dump_size + 1;
                });
            }
            break;
        } else {
            break;
        }
    }

    if (pool->end_at <= now()) {
        auto balance_finaltable = pool->eos_balance * FINAL_TABLE_PERCENT;
        auto balance_jackpot = pool->eos_balance - balance_finaltable;

        // pay the jackpot winner
        auto winner = pool->lastbuyer;
        auto itr_winner = accounts.find(winner);
        if (itr_winner != accounts.end()) {
            accounts.modify(itr_winner, _self, [&](auto &p){
                p.eos_balance += balance_jackpot;
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
        auto itr2 = finaltable.begin();
        while (itr2 != finaltable.end()) {
            // final check on each user's final table chips
            if (itr2->end <= dump_size) {
                auto itr_fter = accounts.find(itr2->owner);
                if (itr_fter != accounts.end()) {
                    uint64_t reduced_keys = itr2->end - itr2->start + 1;
                    accounts.modify(itr_fter, _self, [&](auto &p){
                        p.finaltable_keys -= reduced_keys;
                    });
                }
            } else if (itr2->start <= dump_size) {
                auto itr_fter = accounts.find(itr2->owner);
                if (itr_fter != accounts.end()) {
                    accounts.modify(itr_fter, _self, [&](auto &p){
                        p.finaltable_keys -= (dump_size - itr2->start + 1);
                    });
                }
                break;
            }
            itr2 = finaltable.erase(itr2);
        }

        // pay tournament winners on final table
        // update accounts key_balance for new round

        for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
            accounts.modify(itr, _self, [&](auto &p){
                //uint64_t newkeybal = p.key_balance * KEY_CARRYOVER;
                p.key_balance = 0;
                uint64_t ftprize = balance_finaltable * ((double)p.finaltable_keys / (double)finaltable_size);
                p.eos_balance += ftprize;
                p.finaltable_keys = 0;

                // TODO: clearing all here is better?
                p.ref_balance = 0;
                p.bonus_balance = 0;
                p.ft_balance = 0;
            });
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
            p.end_at = now() + 24 * 3600;
            //p.end_at = now() + 60;
            p.last_buy_ts = now();
            p.key_balance = 0;
            p.eos_balance = p.bonus_balance;
            p.key_price = pricemap[0][1];
            p.eos_total = 0;
            p.dividend_paid = 0;
            p.bonus_balance = 0;
            p.bonus_keys_needed = pricemap[0][0];
            p.total_time_in_sec = 0;
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

    auto user = t.from;
    if (t.quantity.amount == 10) {
        auto owner_refs = referrals.get_index<N(byowner)>();
        auto ref_itr = owner_refs.find(user);
        eosio_assert(ref_itr == owner_refs.end(), "User already registered");
        referrals.emplace(_self, [&](auto &p){
            p.id = referrals.available_primary_key();
            p.owner = name{user};
        });
        return;
    }

    // find pool
    auto pool = pools.begin();
    if (pool == pools.end()) {
        print(">>> no pool is found");
        return;
    }

    string usercomment = t.memo;
    name referee_name = name{TEAM_NAME};
    // check if user comes from referral or not
    if (usercomment.find("ref:0x") == 0) {
        uint32_t pos = usercomment.find(":ref");
        if (pos > 0) {
            string ucm = usercomment.substr(6, pos - 6);
            print(ucm);
            uint64_t refn = stoi(ucm);
            print(">>> referee id:", refn);
            auto itr_refn = referrals.find(refn);
            if (itr_refn != referrals.end()) {
                referee_name = itr_refn->owner;
                eosio_assert(referee_name != user, "Referring yourself is not allowed.");
                usercomment = usercomment.substr(pos + 4);
                print(">>> referee:", referee_name);
                print(">>> comment", usercomment);
            }
        }
    }

    auto amount = t.quantity.amount;
    uint64_t keybal = pool->key_balance;
    uint64_t cur_price = get_price(keybal);
    uint64_t keycnt = amount / cur_price;
    uint64_t newkeycnt = keybal + keycnt;
    uint64_t new_price = get_price(newkeycnt);

    // pay dividend
    uint64_t dividend = accounts.begin() == accounts.end() ? 0 : (amount * DIVIDEND_PERCENT);
    uint64_t dividend_paid = 0;
    for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
        auto share = dividend * ((double)itr->key_balance / (double)keybal);
        dividend_paid += share;
        accounts.modify(itr, _self, [&](auto &p){
            p.eos_balance += share;
        });
    }

    // add or update user
    auto itr_user = accounts.find(user);
    if (itr_user == accounts.end()) {
        print(">>> add account: ", user);
        itr_user = accounts.emplace(_self, [&](auto &p){
            p.owner = name{user};
            p.key_balance = 0;
            p.eos_balance = 0;
            p.ref_balance = 0;
            p.bonus_balance = 0;
            p.ft_balance = 0;
            p.finaltable_keys = 0;
            p.referee = referee_name;
        });
    }

    // check if the user win the bonus prize
    if (pool->bonus_keys_needed < keycnt ) {
        // congrats to the bonus winner!
        uint64_t sweetiebonus = pool->bonus_balance;
        uint64_t next_level_keys = get_level_keys(newkeycnt);

        pools.modify(pool, _self,  [&](auto &p) {
           p.dividend_paid += sweetiebonus;
           p.bonus_balance = 0;
           p.bonus_keys_needed = next_level_keys;
        });
        accounts.modify(itr_user, _self, [&](auto &p){
            p.eos_balance += sweetiebonus;
        });
    }

    // update user keys
    accounts.modify(itr_user, _self, [&](auto &p){
        p.key_balance += keycnt;
        p.finaltable_keys += keycnt;
    });
    // update final table
    finaltable.emplace(_self, [&](auto &p){
        p.start = keybal + 1;
        p.end = newkeycnt;
        p.owner = name{user};
    });

    // pay referral
    auto itr_referee = accounts.find(itr_user->referee);
    uint64_t ref_bonus = 0;
    if (itr_referee != accounts.end()) {
        ref_bonus = amount * REFERRAL_PERCENT;
        accounts.modify(itr_referee, _self, [&](auto &p){
            p.ref_balance += ref_bonus;
            p.eos_balance += ref_bonus;
        });
    }

    // update pool with final prize and bonus prize
    uint64_t prize_share = amount * FINAL_PRIZE_PERCENT;
    uint64_t bonus_share = amount * BONUS_PRIZE_PERCET;
    pools.modify(pool, _self,  [&](auto &p) {
        p.lastbuyer = name{user};
        p.lastcomment = string(usercomment);
        p.last_buy_ts = now();
        p.end_at = std::min(p.end_at + TIME_INC, p.last_buy_ts + DAY_IN_SEC);

        p.key_balance += keycnt;
        p.eos_balance += prize_share;
        if (p.key_price != new_price) {
            p.key_price = new_price;
        }
        p.eos_total += amount;
        p.dividend_paid += (ref_bonus + dividend);
        p.bonus_balance += bonus_share;
        p.bonus_keys_needed -= keycnt;
        p.total_time_in_sec += TIME_INC;
    });

    // pay team if all above steps succeed
    uint64_t team_share = amount - dividend_paid - ref_bonus - prize_share - bonus_share;
    auto team = accounts.find(name{TEAM_NAME});
    if (team == accounts.end()) {
        team = accounts.emplace(_self, [&](auto &p) {
            p.owner = name{TEAM_NAME};
            p.key_balance = 0;
            p.eos_balance = 0;
            p.ref_balance = 0;
            p.bonus_balance = 0;
            p.ft_balance = 0;
            p.finaltable_keys = 0;
            p.referee = name{TEAM_NAME};
        });
    }
    accounts.modify(team, _self, [&](auto &p) {
       p.eos_balance += team_share;
    });
    print("==============================end");
}

//void scam::runwithdraw() {
void scam::runwithdraw(const scam::st_withdraw &toaccount) {
    name to = toaccount.to;
    print(">>> runwithdraw:", name{to});

    // find user
    auto itr = accounts.find(to);
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

    auto itr_ref = referrals.begin();
    while (itr_ref != referrals.end()) {
        itr_ref = referrals.erase(itr_ref);
    }

    auto itr_ft = finaltable.begin();
    while (itr_ft != finaltable.end()) {
        itr_ft = finaltable.erase(itr_ft);
    }
}

void scam::reset() {
    require_auth(_self);
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
             currency::transfer tr = unpack_action_data<currency::transfer>(); \
             if (tr.to == self) { \
                 thiscontract.deposit(tr, code); \
             } \
             return; \
         } \
         if (action == N(pong2)) { \
            thiscontract.runwithdraw(unpack_action_data<scam::st_withdraw>()); \
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

EOSIO_ABI_EX(scam, (pong2)(ping)(pong)(createpool)(deleteall)(reset))
