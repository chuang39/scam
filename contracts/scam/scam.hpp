//
// Created by Kevin Huang on 7/20/18.
//

#ifndef PROJECT_SCAM_H
#define PROJECT_SCAM_H
#endif //PROJECT_SCAM_H

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
            transactions(_self,_self),
            pools(_self, _self){};

    struct st_scam_config {
        uuid     last_id = 0;
        asset    creation_fee = asset{0,S(4,EOS)};
        uint8_t  max_health = 100;
        uint32_t hunger_to_zero = 10 * HOUR;
        uint32_t min_hunger_interval = 3 * HOUR;
        uint8_t  max_hunger_points = 100;
        uint8_t  hunger_hp_modifier = 1;
        uint32_t min_awake_interval = 8 * HOUR;
        uint32_t min_sleep_period = 4 * HOUR;
        uint32_t creation_tolerance = 1 * HOUR;
        uint32_t monsters_to_activate_fee = 1000;
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

    typedef multi_index<N(transactions), st_transactions,
            indexed_by<N(byowner), const_mem_fun<st_transactions, uint64_t, &st_transactions::get_transactions_by_owner>>
    > _tb_transactions;

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

    _tb_transactions transactions;
    _tb_pools pools;

    void createtran(account_name owner, string pet_name);

    void createpool(account_name owner, string poolname);
    void getpool(account_name owner);

};