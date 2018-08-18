#include "scam.hpp"

/*
 *
 * [̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅]
 * Maybe you think Trump's scam is O(scary), but seems quick enough in practice.
 *
 * As our greatest president once said: "Dude...dude... my IQ is one of the highest -and you all know it!
 * Please don't feel so stupid and insecure , it's not your fault!"
 * [̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅][̲̅$̲̅(̲̅100̲̅)̲̅$̲̅]
 *
 */

//#define DEBUG 1
// What's this table used for? God knows!
uint64_t pricetable[8][2] = {{100000 * 16, 100},
                                {100000 * 48, 162},
                                {100000 * 96, 262},
                                {100000 * 192, 424},
                                {100000 * 384, 685},
                                {100000 * 768, 1109},
                                {100000 * 1536, 1794},
                                {100000 * 3072, 2903}};

uint64_t get_price(uint64_t sold_keys) {
    for (int i = 0; i < 8; i++) {
        if (sold_keys < pricetable[i][0]) {
            return pricetable[i][1];
        }
    }
    return 4697;
}

// Oops..
/*
uint64_t get_level_keys(uint64_t sold_keys) {
    for (int i = 0; i < 8; i++) {
        if (sold_keys < pricetable[i][0]) {
            return pricetable[i][0] / 2;
        }
    }
    return pricetable[7][0] / 2;
}
*/

// it's a ping from Mr. Jinping Xi. I like ping-ping-ping.
void scam::ping() {
    require_auth(_self);
    checkpool();
}

// yep, it's a pong from President Kim Jong Un. I like pong-pong-pong.
void scam::pong() {
    require_auth(_self);
    print("Hello Mr. Trump..");
}

// why there is another pong?
void scam::pong2(const name to) {
    require_auth(to);
}

// Start of the gambling.. I will make SO MANY of the jobs!
void scam::createpool(const name owner, const string poolname, const uint32_t starttime) {
    require_auth(_self);

#ifdef DEBUG
    print( "Create pool ", poolname, " by owner= ", name{owner} );
#endif

    pools.emplace(owner, [&](auto &pool) {
        pool.id = pools.available_primary_key();
        pool.poolname = string(poolname);
        pool.owner = name{owner};
        pool.lastbuyer = name{owner};
        pool.status = 1;
        pool.round = 1;
        pool.created_at = starttime;
        pool.end_at = starttime + DAY_IN_SEC;
        pool.last_buy_ts = now();
        pool.key_balance = 0;
        pool.eos_balance = 0;
        pool.key_price = pricetable[0][1];
        pool.eos_total = 0;
        pool.dividend_paid = 0;
        pool.total_time_in_sec = 0;
    });
#ifdef DEBUG
    for( const auto& pool : pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner);
    }
#endif
}

// let us check if the ship is broken or not.
void scam::checkpool() {
    auto pool = pools.begin();
    eosio_assert(pool != pools.end(), "No pool is found");
    eosio_assert(now() >= pool->created_at, "Game hasn't started yet. Please keep patient.");

#ifdef DEBUG
    print(">>> current time: ", now());
    print(">>> found the poolname: ", pool->poolname);
    print(">>> found the round: ", pool->round);
    print(">>> found the onwer: ", pool->owner);
    print(">>> found the created_at: ", pool->created_at);
    print(">>> found the end_at: ", pool->end_at);
    print(">>> found the key_balance: ", pool->key_balance);
    print(">>> found the eos_balance: ", pool->eos_balance);
#endif

    if (pool->end_at <= now()) {
        // Get the number of key we hold and discard for finaltable
        uint64_t finaltable_size = pool->key_balance * FINAL_TABLE_PORTION;
        uint64_t dump_size = pool->key_balance - finaltable_size;
        // TODO: just joking ┬┴┬┴┤( ͡° ͜ʖ├┬┴┬┴┬┴┬┴┤( ͡° ͜ʖ├┬┴┬┴┬┴┬┴┤( ͡° ͜ʖ├┬┴┬┴┬
        // Get the balance for jackpot and final table
        uint64_t balance_finaltable = (uint64_t)(pool->eos_balance * FINAL_TABLE_PERCENT /
                                                         (FINAL_TABLE_PERCENT + FINAL_PRIZE_PERCENT));
        uint64_t balance_jackpot = pool->eos_balance - balance_finaltable;

        // pay the jackpot winner
        auto winner = pool->lastbuyer;
        auto itr_winner = accounts.find(winner);
        if (itr_winner != accounts.end()) {
            accounts.modify(itr_winner, _self, [&](auto &p){
                eosio_assert(p.eos_balance + balance_jackpot >= p.eos_balance,
                             "integer overflow on user eos balance");
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
            }
            itr2 = finaltable.erase(itr2);
        }

        // pay tournament winners on final table
        // update accounts key_balance for new round

        for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
            accounts.modify(itr, _self, [&](auto &p){
                // No key carray ove cross round.. coming feature
                //uint64_t newkeybal = p.key_balance * KEY_CARRYOVER;
                p.key_balance = 0;
                uint64_t ftprize = balance_finaltable * ((double)p.finaltable_keys / (double)finaltable_size);
                eosio_assert(p.eos_balance + ftprize >= p.eos_balance,
                             "integer overflow on user eos balance!!");
                p.eos_balance += ftprize;   // User balance is kept safe and sound.
                p.finaltable_keys = 0;

                // TODO: is full cleanup better?
                p.ref_balance = 0;
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
            // TODO: plan to add one day cooling time here
            p.created_at = now();
            p.end_at = now() + DAY_IN_SEC;
            p.last_buy_ts = now();
            p.key_balance = 0;
            p.eos_balance = 0;
            p.key_price = pricetable[0][1];
            p.eos_total = 0;
            p.dividend_paid = 0;
            p.total_time_in_sec = 0;
        });
    }
}

// Hurray!!!! I got you!!!
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

    // pay dividend
    uint64_t dividend = accounts.begin() == accounts.end() ? 0 : (amount * DIVIDEND_PERCENT);
    uint64_t dividend_paid = 0;
    for (auto itr = accounts.begin(); itr != accounts.end(); itr++) {
        uint64_t share = (uint64_t)(dividend * ((double)itr->key_balance / (double)keybal));
        dividend_paid += share;
        accounts.modify(itr, _self, [&](auto &p){
#ifdef DEBUG
            print(">>> current balance: ", p.eos_balance);
            print(">>> new dividend: ", share);
#endif
            eosio_assert(p.eos_balance + share >= p.eos_balance,
                         "integer overflow on user eos balance!!!");
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
            p.ft_balance = 0;
            p.finaltable_keys = 0;
            p.referee = referee_name;
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
            eosio_assert(p.eos_balance + ref_bonus >= p.eos_balance,
                         "integer overflow on user eos balance!!!!");
            p.eos_balance += ref_bonus;
        });
    }

    // update pool with final prize
    uint64_t prize_share = amount * (FINAL_PRIZE_PERCENT + FINAL_TABLE_PERCENT);
    pools.modify(pool, _self,  [&](auto &p) {
        p.lastbuyer = name{user};
        p.lastcomment = string(usercomment);
        p.last_buy_ts = now();
        p.end_at = std::min(p.end_at + TIME_INC, p.last_buy_ts + DAY_IN_SEC);

        eosio_assert(p.key_balance + keycnt >= p.key_balance,
                     "integer overflow on pool total key!");
        p.key_balance += keycnt;
        eosio_assert(p.eos_balance + prize_share >= p.eos_balance,
                     "integer overflow on pool total eos prize!");
        p.eos_balance += prize_share;
        if (p.key_price != new_price) {
            p.key_price = new_price;
        }
        eosio_assert(p.eos_total + amount >= p.eos_total,
                     "integer overflow on pool total received eos!");
        p.eos_total += amount;
        p.dividend_paid += (ref_bonus + dividend);
        p.total_time_in_sec += TIME_INC;
    });

    // pay team if all above steps succeed
    uint64_t team_share = amount - dividend_paid - ref_bonus - prize_share;
    auto team = accounts.find(name{TEAM_NAME});
    if (team == accounts.end()) {
        team = accounts.emplace(_self, [&](auto &p) {
            p.owner = name{TEAM_NAME};
            p.key_balance = 0;
            p.eos_balance = 0;
            p.ref_balance = 0;
            p.ft_balance = 0;
            p.finaltable_keys = 0;
            p.referee = name{TEAM_NAME};
        });
    }
    accounts.modify(team, _self, [&](auto &p) {
       p.eos_balance += team_share;
    });
}

// Thanks for making America great again!
void scam::runwithdraw(const scam::st_withdraw &toaccount) {
    name to = toaccount.to;
    print(">>> runwithdraw:", name{to});

    // find user
    auto itr = accounts.find(to);
    eosio_assert(itr != accounts.end(), "User not found");
    eosio_assert(itr->eos_balance > 0, "Balance is not greater than zero.");

    // clear balance
    asset bal = asset(itr->eos_balance, symbol_type(S(4, EOS)));
    accounts.modify(itr, _self, [&](auto &p) {
        p.eos_balance = 0;
    });

    // withdraw
    action(permission_level{_self, N(active)}, N(eosio.token),
           N(transfer), std::make_tuple(_self, to, bal,
                                        std::string("Money exits from TrumpScam.me!")))
            .send();
}

// A nuclear button I hate to use.. scary?!
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

// Just scare you
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
/*
 * For the brave souls who get this far: You are the chosen ones,
 * the valiant knights of programming who toil away, without rest,
 * fixing our most awful code. To you, true saviors, kings of men,
 * I say this: never gonna give you up, never gonna let you down,
 * never gonna run around and desert you. Never gonna make you cry,
 * never gonna say goodbye. Never gonna tell a lie and hurt you.
 * - From Internet
*/