#ifndef TRANSACTION_HPP
#define TRANSACTION_HPP

/** A single transaction that may be committed. */
class Transaction {
public:
    /**
     * Attempt to commit the transaction.
     * @return True if the commit succeeded.
     */
    virtual bool commit() = 0;
};

/**
 * Interface for transactional operations.
 * @tparam T The type of the transactions.
 */
template<class T>
class Transactional {
public:
    /**
     * Begin a transaction.
     * @return New transaction object.
     */
    virtual T begin_transaction() = 0;
};

#endif // TRANSACTION_HPP
