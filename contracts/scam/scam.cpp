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


    auto owner_pools = pools.get_index<N(byowner)>();
    auto bfpool = owner_pools.get(N(blockfishbgp));


    print( "Welsome %s! Start purchasing...", name{owner} );
    transactions.emplace(owner, [&](auto &r) {
        st_transactions transaction{};
        transaction.id = transactions.available_primary_key();
        //transaction.name = "baby";
        transaction.owner = name{owner};
        transaction.created_at = now();
        transaction.ammount = 100;

        //pet.type = (hash_str(pet_name) + pet.created_at + pet.id + owner) % pc.last_pet_type_id;

        r = transaction;
    });


}

void scam::createpool(account_name owner, string poolname) {

    require_auth(_self);

    //uuid new_id = _next_id();
    //auto owner_pools = pools.get_index<N(byowner)>();
    //auto pitr = owner_pools.get(name{owner});
    //pools.erase( pitr );

    auto owner_pools = pools.get_index<N(byowner)>();
    auto pitr = owner_pools.find(name{owner});
    if(pitr != owner_pools.end()) {
        owner_pools.erase(pitr);
    }

    for( const auto& pool : pools ) {
        print(" ID=", pool.id, ", owner:", pool.owner, ", ammount: ",
              pool.ammount, ", end_at:",
              pool.end_at, ", created_at:", pool.created_at, "\n");
    }
    auto owner_pools = pools.get_index<N(byowner)>();
    for( const auto& pool : owner_pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner, ", ammount: ",
              pool.ammount, ", end_at:",
              pool.end_at, ", created_at:", pool.created_at, "\n");
    }


    print( "=============createpool:, ", name{owner} );
    /*
    pools.emplace(owner, [&](auto &r) {
        st_pools pool{};
        pool.id = 0;
        //transaction.name = "baby";
        pool.owner = name{owner};
        pool.created_at = now();
        pool.end_at = now() + 24 * 3600;
        pool.ammount = 10;
        r = pool;
    });
     */


}

void scam::getpool(account_name owner) {
    print("Items sorted by primary key:\n");
    for( const auto& pool : pools ) {
        print(" ID=", pool.id, ", owner:", pool.owner, ", ammount: ",
              pool.ammount, ", end_at:",
              pool.end_at, ", created_at:", pool.created_at, "\n");
    }
}

EOSIO_ABI( scam, (createtran)(createpool)(getpool))