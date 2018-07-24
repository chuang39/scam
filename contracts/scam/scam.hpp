//
// Created by Kevin Huang on 7/20/18.
//

#ifndef PROJECT_SCAM_H
#define PROJECT_SCAM_H
#endif //PROJECT_SCAM_H

#include <math.h>
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
            :contract(self){};

    void createtran(const account_name from, const asset& quantity);
};
/*
class scam : public eosio::contract {
  public:
    scam(account_name self)
            :contract(self),
            transactions(_self,_self),
            pools(_self, _self),
            accounts(_self, _self){};

    struct st_scam_config {
        asset    creation_fee = asset{0,S(4,EOS)};
    };

    struct st_accounts {
        name owner;
        uint32_t created_at;
        uint32_t ammount;
        uint32_t bets;

        uint64_t primary_key() const { return owner; }

        uint64_t get_transactions_by_owner() const { return owner.value; }
        EOSLIB_SERIALIZE(st_accounts, (owner)(created_at)(ammount)(bets) )
    };

    struct st_transactions {
        //uuid id;
        uint32_t id;
        name owner;
        //string name;
        uint32_t created_at;
        uint32_t ammount;

        uint64_t primary_key() const { return id; }

        uint64_t get_transactions_by_owner() const { return owner.value; }
        //EOSLIB_SERIALIZE( game, (challenger)(host)(turn)(winner)(board))
    };

    struct st_pools {
        //uuid id;
        uint32_t id;
        // TODO: use name
        //string name;
        name owner;
        uint32_t created_at;
        uint32_t end_at;
        uint32_t ammount;

        uint64_t primary_key() const { return id; }

        uint64_t get_pools_by_owner() const { return owner; }
        //EOSLIB_SERIALIZE( game, (challenger)(host)(turn)(winner)(board))
    };

    typedef multi_index<N(pools), st_pools,
            indexed_by<N(byowner), const_mem_fun<st_pools, uint64_t, &st_pools::get_pools_by_owner>>
    > _tb_pools;

    typedef multi_index<N(transactions), st_transactions,
            indexed_by<N(byowner), const_mem_fun<st_transactions, uint64_t, &st_transactions::get_transactions_by_owner>>
    > _tb_transactions;

    typedef multi_index<N(accounts), st_accounts> _tb_accounts;

    _tb_transactions transactions;
    _tb_pools pools;
    _tb_accounts accounts;

    void createtran(const account_name from, const asset& quantity);

    void createpool(account_name owner, string poolname);
    void getpool(account_name owner);

};
 */