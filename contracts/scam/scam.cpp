//
// Created by Kevin Huang on 7/20/18.
//

#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include "scam.h"


class dice : public eosio::contract {

public:
    struct st_transactions {
        uuid id;
        name owner;
        string name;
        uint32_t created_at;
        uint32_t ammount;

        uint64_t primary_key() const { return id; }

        uint64_t get_transactions_by_owner() const { return owner.value; }
    };


    typedef multi_index<N(transactions), st_transactions,
            indexed_by<N(byowner), const_mem_fun<st_transactions, uint64_t, &st_transactions::get_transactions_by_owner>>
    > _tb_transactions;

    _tb_transactions transactions;

    void createpet(name owner, string pet_name) {

        require_auth(owner);

        transactions.emplace(owner, [&](auto &r) {
            st_transactions transaction{};
            transaction.id = new_id;
            transaction.name = "baby";
            transaction.owner = owner;
            transaction.created_at = now();
            transaction.ammount = 0;

            //pet.type = (hash_str(pet_name) + pet.created_at + pet.id + owner) % pc.last_pet_type_id;

            r = transaction;
        });


        auto owner_transactions = transactions.get_index<N(byowner)>();
    }
};