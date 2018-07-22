//
// Created by Kevin Huang on 7/20/18.
//

#include "scam.hpp"

void scam::createtran(const account_name from, const asset& quantity) {

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must deposit positive quantity" );
    //eosio_assert(code == N(eosio.token), "I reject your non-eosio.token deposit");
    eosio_assert(quantity.symbol == string_to_symbol(4, "EOS"),
                 "only accepts EOS for deposits");

    //auto owner_pools = pools.get_index<N(byowner)>();
    //auto pitr = owner_pools.get(N(blockfishbgp));
    //print(" ~~ID=", pitr.id, ", owner:", pitr.owner, ", ammount: ",
    //      pitr.ammount, ", end_at:",
    //      pitr.end_at, ", created_at:", pitr.created_at, "\n");


    auto aitr = accounts.find(from);
    if(aitr == accounts.end()) {
        aitr = accounts.emplace(_self, [&](auto& account){
            transaction.owner = name{from};
            transaction.created_at = now();
        });
        print(" ~~~~~~~~~ID=", aitr.id, ", owner:", aitr.owner, ", ammount: ",
              aitr.ammount, ",  created_at:", aitr.created_at, "\n");
    }

    action(
            permission_level{ from, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple(from, _self, quantity, std::string(""))
    ).send();

    accounts.modify(aitr, 0, [&](auto &r) {
        r.ammount += ceil(quantity.amount / 0.5);
    });


}

void scam::createpool(account_name owner, string poolname) {

    require_auth(_self);

    auto owner_pools = pools.get_index<N(byowner)>();
    auto pitr = owner_pools.find(name{owner});
    owner_pools.erase( pitr );
    print( "Create pool by owner=%s ", name{owner} );

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

    for( const auto& pool : pools ) {
        print(" ID=", pool.id, ", owner:", pool.owner, ", ammount: ",
              pool.ammount, ", end_at:",
              pool.end_at, ", created_at:", pool.created_at, "\n");
    }
    owner_pools = pools.get_index<N(byowner)>();
    for( const auto& pool : owner_pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner, ", ammount: ",
              pool.ammount, ", end_at:",
              pool.end_at, ", created_at:", pool.created_at, "\n");
    }

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