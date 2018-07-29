//
// Created by Kevin Huang on 7/20/18.
//

#include "scam.hpp"

void scam::createacnt2(string name, string city, uint32_t zipcode,
                      uint8_t rating, uint8_t type, string logo,
                      string picture, string website, string phone) {
    accounts2.emplace(_self, [&](auto& account){
            account.id = accounts2.available_primary_key();
            account.name = name;
            account.city = city;
            account.rating = 0;
            account.type = type;
            account.zipcode = zipcode;
            account.logo = logo;
            account.picture = picture;
            account.website = website;
            account.phone = phone;
            account.num_revs = 0;
        });
}

void scam::createrevw2(string user, string business, uint32_t rating, string line) {
    reviews2.emplace(_self, [&](auto& review){
            review.id = reviews2.available_primary_key();
            review.user = user;
            review.business = business;
            review.rating = rating;
            review.line = line;
            review.created_at = now();
        });
}
void scam::deleterevw() {
    for( const auto& account : accounts2 ) {
        accounts2.erase(account);
    }
    for( const auto& review : reviews2 ) {
        reviews2.erase(review);
    }

}
EOSIO_ABI( scam, (createacnt2)(createrevw2)(deleterevw))

/*
void scam::createtran(const account_name from, const asset& quantity) {

    eosio_assert( quantity.is_valid(), "invalid quantity" );
    eosio_assert( quantity.amount > 0, "must deposit positive quantity" );
    //eosio_assert(code == N(eosio.token), "I reject your non-eosio.token deposit");
    eosio_assert(quantity.symbol == string_to_symbol(4, "EOS"),
                 "only accepts EOS for deposits");

    // get pool. Check if the pool is expired or not.
    auto owner_pools = pools.get_index<N(byowner)>();
    auto pitr = owner_pools.get(N(blockfishbgp));
    eosio_assert(pitr != owner_pools.end(), "E404|Invalid pool");
    eosio_assert(pitr->status == 1, "E404|The scam has ended.");

    auto aitr = accounts.find(from);
    if(aitr == accounts.end()) {
        aitr = accounts.emplace(_self, [&](auto& account){
            account.owner = name{from};
            account.created_at = now();
        });
        print(" ~~~~~~~~~owner:", aitr->owner, ", ammount: ",
              aitr->ammount, ",  created_at:", aitr->created_at, "\n");
    }

    action(
            permission_level{ from, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple(from, _self, quantity, std::string(""))
    ).send();

    // add key to user account
    accounts.modify(aitr, 0, [&](auto &r) {
        r.key_balance += ceil(quantity.amount / 0.5);
    });

    // update pool
    pitr->eos

    //print(" ~~ID=", pitr.id, ", owner:", pitr.owner, ", ammount: ",
    //      pitr.ammount, ", end_at:",
    //      pitr.end_at, ", created_at:", pitr.created_at, "\n");


    // update dividend for account

    //auto owner_pools = pools.get_index<N(byowner)>();
    //auto pitr = owner_pools.get(N(blockfishbgp));
    //print(" ~~ID=", pitr.id, ", owner:", pitr.owner, ", ammount: ",
    //      pitr.ammount, ", end_at:",
    //      pitr.end_at, ", created_at:", pitr.created_at, "\n");


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

EOSIO_ABI( scam, (createtran)(createpool)(getpool)(ping))
*/