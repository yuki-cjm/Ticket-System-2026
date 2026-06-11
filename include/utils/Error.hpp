#pragma once

#include <cstring>
#include <string>

class TicketSystemError {
   protected:
    const std::string variant = "";
    std::string detail = "";

   public:
    TicketSystemError() {
    }
    TicketSystemError(const TicketSystemError &ec) : variant(ec.variant), detail(ec.detail) {
    }
    virtual std::string what() {
        return variant + " " + detail;
    }
};

class index_out_of_bound : public TicketSystemError {
    /* __________________________ */
};

class runtime_error : public TicketSystemError {
    /* __________________________ */
};

class invalid_iterator : public TicketSystemError {
    /* __________________________ */
};

class container_is_empty : public TicketSystemError {
    /* __________________________ */
};

class unknown_order : public TicketSystemError {
    /* __________________________ */
};