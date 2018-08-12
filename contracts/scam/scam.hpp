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


#define EOSIO_ABI_EX( TYPE, MEMBERS ) \
extern "C" { \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      auto self = receiver; \
      if( code == self || code == N(eosio.token) || action == N(onerror) ) { \
         TYPE thiscontract(self); \
         switch( action ) { \
            EOSIO_API( TYPE, MEMBERS ) \
         } \
         eosio_exit(0); \
      } \
   } \
}



class scam : public eosio::contract {
  public:
    scam(account_name self)
            :contract(self),
             pools(_self, _self),
             accounts(_self, _self){};

    void deposit(const currency::transfer &t, account_name code);
    void withdraw(const account_name to);
    void runwithdraw(const account_name to);
    void createpool(const name owner, const string poolname);
    void deleteall();
    void reset();
    void apply(account_name contract, account_name act);

    struct st_withdraw {
        account_name to;
    };

  private:
    // TODO: don't expose end_at in the table
    // TODO: create read method for table filed (limit access)
    // TODO: withdraw
    // TODO: guarantee the price and key bundle
    // @abi table pools i64
    struct st_pools {
        uint64_t id;
        string poolname;
        name owner;
        name lastbuyer;
        uint8_t status; // 0 for inactive; 1 for active
        uint32_t created_at;
        uint32_t end_at;
        uint32_t key_balance;
        asset eos_balance;
        asset key_price;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(st_pools, (id)(poolname)(owner)(status)(created_at)(end_at)(key_balance)(eos_balance)(key_price))
    };
    typedef multi_index<N(pools), st_pools> _tb_pools;
    _tb_pools pools;

    // @abi table accounts i64
    struct st_accounts {
        name owner;
        uint32_t key_balance;
        asset eos_balance;
        uint64_t primary_key() const {return owner;}
        EOSLIB_SERIALIZE(st_accounts, (owner)(key_balance)(eos_balance))
    };
    typedef multi_index<N(accounts), st_accounts> _tb_accounts;
    _tb_accounts accounts;
};
