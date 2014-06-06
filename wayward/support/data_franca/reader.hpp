#pragma once
#ifndef WAYWARD_SUPPORT_DATA_FRANCA_READER_HPP_INCLUDED
#define WAYWARD_SUPPORT_DATA_FRANCA_READER_HPP_INCLUDED

#include <wayward/support/data_franca/types.hpp>
#include <wayward/support/data_franca/get_adapter.hpp>

#include <wayward/support/maybe.hpp>

#include <vector>
#include <memory>

namespace wayward {
  namespace data_franca {
    struct IReaderEnumerator;
    using ReaderEnumeratorPtr = std::unique_ptr<IReaderEnumerator>;

    /*
      A Reader traverses data and inspects it as it passes over it.
      It has the ability to dig down into data structures.
    */
    struct IReader {
      virtual ~IReader() {}

      virtual DataType type() const = 0;

      virtual Maybe<Boolean> get_boolean() const = 0;
      virtual Maybe<Integer> get_integer() const = 0;
      virtual Maybe<Real>    get_real()    const = 0;
      virtual Maybe<String>  get_string()  const = 0;

      virtual bool has_key(const String& key) const = 0;
      virtual ReaderPtr get(const String& key) const = 0;

      virtual size_t   length()       const = 0;
      virtual ReaderPtr at(size_t idx) const = 0;

      virtual ReaderEnumeratorPtr enumerator() const = 0;
    };

    struct IReaderEnumerator {
      virtual ~IReaderEnumerator() {}
      virtual ReaderPtr current_value() const = 0;
      virtual Maybe<String> current_key() const = 0;
      virtual bool at_end() const = 0;
      virtual void move_next() = 0;
    };

    struct NullReader : IReader {
      DataType type() const final { return DataType::Nothing; }
      Maybe<Boolean> get_boolean() const final { return Nothing; }
      Maybe<Integer> get_integer() const final { return Nothing; }
      Maybe<Real>    get_real()    const final { return Nothing; }
      Maybe<String>  get_string()  const final { return Nothing; }
      bool has_key(const String& key) const { return false; }
      std::vector<String> keys() const { return {}; }
      ReaderPtr get(const String& key) const { return nullptr; }
      size_t   length()       const { return 0; }
      ReaderPtr at(size_t idx) const { return nullptr; }
      ReaderEnumeratorPtr enumerator() const { return nullptr; }
    };

    struct ReaderEnumeratorAtEnd : IReaderEnumerator {
      ReaderPtr current_value() const final { return nullptr; }
      Maybe<String> current_key() const final { return Nothing; }
      bool at_end() const final { return true; }
      void move_next() const final {}
    };

    template <typename Self, typename Subscript = Self>
    struct ReaderInterface {
      bool is_nothing() const;
      operator bool() const;
      bool operator>>(Boolean& b) const;
      bool operator>>(Integer& n) const;
      bool operator>>(Real& r) const;
      bool operator>>(String& str) const;
      Subscript operator[](size_t idx) const;
      Subscript operator[](const String& key) const;
      size_t length() const;
      struct iterator;
      iterator begin() const;
      iterator end()   const;

    protected:
      ReaderInterface() {}
    private:
      // Could be an IReader, could be something else...
      auto reader() const -> decltype(std::declval<const Self>().reader_iface()) {
        return static_cast<const Self*>(this)->reader_iface();
      }
    };

    template <typename Self, typename Subscript>
    struct ReaderInterface<Self, Subscript>::iterator {
      bool operator==(const iterator& other) const {
        return (enumerator_ == nullptr && other.enumerator_ == nullptr) || (enumerator_->at_end() == other.enumerator_->at_end());
      }
      bool operator!=(const iterator& other) const { !(*this == other); }
      const Self& operator*() const { return current_; }
      const Self* operator->() const { return &current_; }

      iterator& operator++() { enumerator_->move_next(); current_ = Self{enumerator_->current_value}; return *this; }
    private:
      iterator(ReaderEnumeratorPtr e) : enumerator_{std::move(e)}, current_{enumerator_->current_value()} {}
      ReaderEnumeratorPtr enumerator_;
      const Self current_;
    };

    template <typename Self, typename Subscript>
    bool ReaderInterface<Self, Subscript>::is_null() const {
      return !q_;
    }

    template <typename Self, typename Subscript>
    bool ReaderInterface<Self, Subscript>::is_nothing() const {
      return reader().type() == DataType::Nothing;
    }

    template <typename Self, typename Subscript>
    ReaderInterface<Self, Subscript>::operator bool() const {
      return !is_nothing();
    }

    template <typename Self, typename Subscript>
    bool ReaderInterface<Self, Subscript>::operator>>(Boolean& b) const {
      if (type() == DataType::Boolean) {
        b = *reader().get_boolean();
        return true;
      }
      return false;
    }

    template <typename Self, typename Subscript>
    bool ReaderInterface<Self, Subscript>::operator>>(Integer& n) const {
      if (type() == DataType::Integer) {
        n = *reader().get_integer();
        return true;
      }
      return false;
    }

    template <typename Self, typename Subscript>
    bool ReaderInterface<Self, Subscript>::operator>>(Real& r) const {
      if (type() == DataType::Real) {
        r = *reader().get_real();
        return true;
      }
      return false;
    }

    template <typename Self, typename Subscript>
    bool ReaderInterface<Self, Subscript>::operator>>(String& str) const {
      if (type() == DataType::String) {
        str = *reader().get_string();
        return true;
      }
      return false;
    }

    template <typename Self, typename Subscript>
    size_t ReaderInterface<Self, Subscript>::length() const {
      return reader().length();
    }

    template <typename Self, typename Subscript>
    Subscript ReaderInterface<Self, Subscript>::operator[](size_t idx) const {
      return reader().at(idx);
    }

    template <typename Self, typename Subscript>
    Subscript ReaderInterface<Self, Subscript>::operator[](const String& key) const {
      return reader().get(key);
    }
  }
}

#endif // WAYWARD_SUPPORT_DATA_FRANCA_READER_HPP_INCLUDED
