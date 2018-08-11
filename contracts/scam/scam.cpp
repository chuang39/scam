//
// Created by Kevin Huang on 7/20/18.
//

#include "scam.hpp"

void scam::createpool(const name owner, const string poolname) {
    require_auth(_self);

    // TODO: should only allow one active pool
    print( "Create pool ", poolname, " by owner= ", name{owner} );

    pools.emplace(owner, [&](auto &pool) {
        pool.id = pools.available_primary_key();
        pool.poolname = string(poolname);
        pool.owner = name{owner};
        pool.created_at = now();
        pool.end_at = now();
        pool.key_balance = 10;
        //r.eos_balance = asset(0, symbol_type(S(4, EOS)));
        //r.key_price = asset(1220, symbol_type(S(4, EOS)));
    });

}

//void scam::deposit(uint64_t sender, uint64_t receiver, ) {
void scam::deposit(const name from, const asset& quantity) {
    print("\n>>> sender >>>", from, " - name: ", name{from});



    // adjust pool: key price








/*
    auto transfer_data = unpack_action_data<st_transfer>();
    if(transfer_data.from == _self || transfer_data.to != _self) {
        return;
    }
    print("\n>>> transfer data quantity >>> ", transfer_data.quantity);

    eosio_assert(transfer_data.quantity.symbol == string_to_symbol(4, "EOS"),
                 "MonsterEOS only accepts EOS for deposits");
    eosio_assert(transfer_data.quantity.is_valid(), "Invalid token transfer");
    eosio_assert(transfer_data.quantity.amount > 0, "Quantity must be positive");

    //_tb_accounts accounts(_self, transfer_data.from);
    _tb_accounts accounts(_self, _self);
    asset new_balance;
    auto itr_balance = accounts.find(transfer_data.quantity.symbol.name());
    if(itr_balance != accounts.end()) {
        accounts.modify(itr_balance, transfer_data.from, [&](auto& r){
            // Assumption: total currency issued by eosio.token will not overflow asset
            r.balance += transfer_data.quantity;
            new_balance = r.balance;
        });
    } else {
        accounts.emplace(transfer_data.from, [&](auto& r){
            r.balance = transfer_data.quantity;
            new_balance = r.balance;
        });
    }

    print("\n", name{transfer_data.from}, " deposited:       ", transfer_data.quantity);
    print("\n", name{transfer_data.from}, " funds available: ", new_balance);
*/
}


EOSIO_ABI( scam, (deposit)(createpool))

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