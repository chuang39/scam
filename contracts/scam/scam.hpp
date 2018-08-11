//
// Created by Kevin Huang on 7/20/18.
//

#ifndef PROJECT_SCAM_H
#define PROJECT_SCAM_H
#endif //PROJECT_SCAM_H

//#include <math.h>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
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
             pools(_self, _self){};


    // TODO: don't expose end_at in the table
    // TODO: create read method for table filed (limit access)
    // TODO: withdraw
    // TODO: guarantee the price and key bundle
    // @abi table pools i64
    struct st_pools {
        uint64_t id;
        string poolname;
        name owner;
        uint8_t status; // 0 for inactive; 1 for active
        uint32_t created_at;
        uint32_t end_at;
        uint32_t key_balance;
        asset eos_balance;
        asset key_price;

        uint64_t primary_key() const { return id; }

        uint64_t get_pools_by_owner() const { return owner; }
        EOSLIB_SERIALIZE(st_pools, (id)(poolname)(owner)(status)(created_at)(end_at)(key_balance)(eos_balance)(key_price))
    };
    typedef multi_index<N(pools), st_pools,
            indexed_by<N(byowner), const_mem_fun<st_pools, uint64_t, &st_pools::get_pools_by_owner>>
    > _tb_pools;
    _tb_pools pools;

    // @abi table accounts i64
    struct st_accounts {
        name owner;
        uint32_t created_at;
        uint32_t key_balance;
        asset eos_balance;
        uint64_t primary_key() const {return owner;}
        EOSLIB_SERIALIZE(st_accounts, (owner)(created_at)(key_balance)(eos_balance))
    };
    typedef multi_index<N(accounts), st_accounts> _tb_accounts;
    _tb_accounts accounts;




    void deposit(const name from, const asset& quantity);
    void createpool(const name owner, const string poolname);
    void withdraw( const account_name to, const asset& quantity);
    void reset();
};
