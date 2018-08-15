//
// Created by Kevin Huang on 7/20/18.
//

#ifndef PROJECT_SCAM_H
#define PROJECT_SCAM_H
#endif //PROJECT_SCAM_H

//#include <math.h>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/currency.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/time.hpp>

using namespace eosio;
using std::string;
using std::hash;


class scam : public eosio::contract {
  public:
    scam(account_name self)
            :contract(self),
             pools(_self, _self),
             accounts(_self, _self){};

    //@abi action
    void createpool(const name owner, const string poolname);
    //@abi action
    void deleteall();
    //@abi action
    void reset();
    //@abi action
    void withdraw(const account_name to);
    //@abi action
    void ping();

    void checkpool();
    void deposit(const currency::transfer &t, account_name code);
    void runwithdraw(const st_withdraw &toaccount);
    void apply(account_name contract, account_name act);

    struct st_withdraw {
        account_name to;
    };

  private:

    const static uint64_t DAY_IN_SEC = 3600 * 24;
    constexpr static uint64_t TIME_INC = 30;
    constexpr static double DIVIDEND_PERCENT = 0.4;
    constexpr static double REFERRAL_PERCENT = 0.1;
    constexpr static double BONUS_PRIZE_PERCET = 0.05;
    constexpr static double FINAL_PRIZE_PERCENT = 0.33;
    constexpr static double KEY_CARRYOVER = 0.1;
    constexpr static account_name TEAM_NAME = N(eosgamesprod);

    // @abi table pools i64
    struct st_pools {
        uint64_t id;
        string poolname;
        name owner;
        name lastbuyer;
        uint8_t status; // 0 for inactive; 1 for active
        uint32_t round;
        uint32_t created_at;
        uint32_t end_at;
        uint32_t last_buy_ts;
        uint64_t key_balance;
        uint64_t eos_balance;   // final prize
        uint64_t key_price;
        uint64_t eos_total;    // total incoming money
        uint64_t dividend_paid;
        uint64_t bonus_balance;
        uint64_t bonus_keys_needed;
        uint64_t total_time_in_sec;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(st_pools, (id)(poolname)(owner)(lastbuyer)(status)(round)
                (created_at)(end_at)(last_buy_ts)(key_balance)(eos_balance)(key_price)(eos_total)
                (dividend_paid)(bonus_balance)(bonus_keys_needed)(total_time_in_sec))
    };
    typedef multi_index<N(pools), st_pools> _tb_pools;
    _tb_pools pools;

    // @abi table accounts i64
    struct st_accounts {
        name owner;
        uint64_t key_balance;
        uint64_t eos_balance;   // total balance: dividend + referral bonus
        uint64_t ref_balance;   // ref bonus
        name referee;

        uint64_t primary_key() const {return owner;}
        EOSLIB_SERIALIZE(st_accounts, (owner)(key_balance)(eos_balance)(ref_balance)(referee))
    };
    typedef multi_index<N(accounts), st_accounts> _tb_accounts;
    _tb_accounts accounts;
};
