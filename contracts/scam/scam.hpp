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
    /*
    scam(account_name self):contract(self)
            ,accounts(_self, _self)
            ,reviews(_self, _self){};

    // @abi table accounts i64
    struct st_accounts {
        uint64_t id;
        string name;
        string city;
        uint32_t zipcode;
        uint8_t rating;
        uint8_t type;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(st_accounts, (id)(name)(city)(zipcode)(rating)(type))
    };

    typedef multi_index<N(accounts), st_accounts> _tb_accounts;
    _tb_accounts accounts;

    // @abi table reviews i64
    struct st_reviews {
        uint64_t id;
        string user;
        string business;
        uint8_t rating;
        string line;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(st_reviews, (id)(user)(business)(rating)(line))
    };

    typedef multi_index<N(reviews), st_reviews> _tb_reviews;
    _tb_reviews reviews;

    void createacnt(string name, string city, uint32_t zipcode, uint8_t rating, uint8_t type);
    void createrevw(string user, string business, uint32_t rating, string line);
    void deleterevw();
*/
    /*
    scam(account_name self):contract(self){};
    void ping();
     */
/*
    scam(account_name self):contract(self)
            ,accounts(_self, _self)
            ,reviews(_self, _self){};

    // @abi table accounts i64
    struct st_accounts {
        uint64_t id;
        string name;
        string city;
        uint32_t zipcode;
        uint8_t rating;
        uint8_t type;
        string logo;
        string picture;
        string website;
        uint8_t num_revs;
        string phone;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(st_accounts, (id)(name)(city)(zipcode)(rating)(type)(logo)(picture)(website)(num_revs)(phone))
    };

    typedef multi_index<N(accounts), st_accounts> _tb_accounts;
    _tb_accounts accounts;

    // @abi table reviews i64
    struct st_reviews {
        uint64_t id;
        string user;
        string business;
        uint8_t rating;
        string line;
        uint32_t created_at;

        uint64_t primary_key() const { return id; }

        EOSLIB_SERIALIZE(st_reviews, (id)(user)(business)(rating)(line)(created_at))
    };

    typedef multi_index<N(reviews), st_reviews> _tb_reviews;
    _tb_reviews reviews;

    void createacnt(string name, string city, uint32_t zipcode, uint8_t rating,
                    uint8_t type, string logo, string picture, string website, string phone);
    void createrevw(string user, string business, uint32_t rating, string line);
    void deleterevw();
    void reserve(string user, string business, string time);

*/

  public:
    scam(account_name self)
            :contract(self),
             pools(_self, _self){};
    // @abi table
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
            indexed_by<N(owner), const_mem_fun<st_pools, uint64_t, &st_pools::get_pools_by_owner>>
    > _tb_pools;
    _tb_pools pools;

    /*
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
        uint32_t key_balance;
        asset eos_balance;

        uint64_t primary_key() const { return owner; }

        EOSLIB_SERIALIZE(st_accounts, (owner)(created_at)(key_balance)(eos_balance))
    };

    struct st_transactions {
        uint32_t id;
        name owner;
        //string name;
        uint32_t created_at;
        uint32_t ammount;

        uint64_t primary_key() const { return id; }

        uint64_t get_transactions_by_owner() const { return owner.value; }
        EOSLIB_SERIALIZE( game, (challenger)(host)(turn)(winner)(board))
    };


    typedef multi_index<N(transactions), st_transactions,
            indexed_by<N(byowner), const_mem_fun<st_transactions, uint64_t, &st_transactions::get_transactions_by_owner>>
    > _tb_transactions;

    typedef multi_index<N(accounts), st_accounts> _tb_accounts;

    _tb_transactions transactions;
    _tb_accounts accounts;
*/
    void deposit(const name from, const asset& quantity);
    void createpool(const name owner, const string poolname);
    //void getpool(account_name owner);
    //void ping(account_name receiver);
};
