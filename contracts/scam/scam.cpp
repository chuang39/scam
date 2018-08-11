#include "scam.hpp"

void scam::createpool(const name owner, const string poolname) {
    require_auth(_self);

    // TODO: should only allow one active pool
    // TODO: add a method to control pool state
    print( "Create pool ", poolname, " by owner= ", name{owner} );

    pools.emplace(owner, [&](auto &pool) {
        pool.id = pools.available_primary_key();
        pool.poolname = string(poolname);
        pool.owner = name{owner};
        pool.status = 1;
        pool.created_at = now();
        pool.end_at = now() + 24 * 3600;
        pool.key_balance = 10;
        pool.eos_balance = asset(0, symbol_type(S(4, EOS)));
        pool.key_price = asset(1220, symbol_type(S(4, EOS)));
    });
    auto owner_pools = pools.get_index<N(byowner)>();
    for( const auto& pool : owner_pools ) {
        print(" ~~ID=", pool.id, ", owner:", pool.owner);
    }
}

//void scam::deposit(uint64_t sender, uint64_t receiver, ) {
//@abi action
void scam::deposit(const name from, const asset& quantity) {
    print("\n>>> sender >>>", from, " - name: ", name{from});

    if(from == _self) {
        return;
    }

    eosio_assert(quantity.symbol == string_to_symbol(4, "EOS"), "Only accepts EOS for deposits");
    eosio_assert(quantity.is_valid(), "Invalid token transfer");
    eosio_assert(quantity.amount > 0, "Quantity must be positive");

    SEND_INLINE_ACTION(*this, transfer, {from, N(active)}, {from, _self, quantity, "deposit"});
}

//@abi action
void withdraw(const account_name to, const asset& quantity) {
/*
    require_auth(to);

    eosio_assert(quantity.is_valid(), "invalid quantity");
    eosio_assert(code == N(eosio.token);
    eosio_assert(quantity.amount > 0, "must withdraw positive quantity");

    auto itr = accounts.find(to);
    eosio_assert(itr != accounts.end(), "unknown account");

    accounts.modify( itr, 0, [&]( auto& acnt ) {
        eosio_assert( acnt.eos_balance >= quantity, "insufficient balance" );
        acnt.eos_balance -= quantity;
    });


    if( itr->is_empty() ) {
        accounts.erase(itr);
    }
    */
}

void scam::reset() {
    auto itr = pools.begin();
    while (itr != pools.end()) {
        itr = pools.erase(itr);
    }
}

EOSIO_ABI( scam, (deposit)(createpool)(reset))
