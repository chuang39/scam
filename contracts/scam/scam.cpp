//
// Created by Kevin Huang on 7/20/18.
//

#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/print.hpp>
#include "scam.hpp"

void scam::createtran(account_name owner, string pet_name) {

    require_auth(owner);

    //uuid new_id = _next_id();

    print( "=============Hello, ", name{owner} );
    transactions.emplace(owner, [&](auto &r) {
        st_transactions transaction{};
        transaction.id = 1;
        //transaction.name = "baby";
        transaction.owner = name{owner};
        transaction.created_at = now();
        transaction.ammount = 0;

        //pet.type = (hash_str(pet_name) + pet.created_at + pet.id + owner) % pc.last_pet_type_id;

        r = transaction;
    });


    auto owner_transactions = transactions.get_index<N(byowner)>();
}

void scam::createpool(account_name owner, string poolname) {

    require_auth(owner);

    //uuid new_id = _next_id();

    print( "=============createpool:, ", name{owner} );
    pools.emplace(owner, [&](auto &r) {
        st_transactions transaction{};
        transaction.id = 1;
        //transaction.name = "baby";
        transaction.owner = name{owner};
        transaction.created_at = now();
        transaction.ammount = 0;

        r = transaction;
    });


}

void scam::getpool(account_name owner) {
    print("Items sorted by primary key:\n");
    for( const auto& pool : pools ) {
        print(" ID=", pool.id, ", owner:", pool.owner, "\n");
    }
}

EOSIO_ABI( scam, (createtran), (createpool), (getpool))