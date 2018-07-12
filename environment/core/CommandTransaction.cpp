#include <unordered_set>

#include "CommandTransaction.hpp"
#include "Map.hpp"

namespace hlt {

/**
 * Check whether the commit can succeed without executing it.
 * @return True if the commit can succeed.
 */
bool CommandTransaction::check() const {
    return true;
}

/**
 * Attempt to commit the transaction.
 * @return True if the commit succeeded.
 */
bool CommandTransaction::commit() {
    return true;
}

}
