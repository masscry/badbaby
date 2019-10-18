#include <value.hpp>

namespace 
{
  
  class number_t final: public bb::value_t
  {
    double value;

    number_t(const number_t&) = delete;
    number_t& operator =(const number_t&) = delete;
    number_t(number_t&&) = delete;
    number_t& operator =(number_t&&) = delete;

  public:

    bb::type_t Type() const override
    {
      return bb::type_t::number;
    }

    double Number() const override
    {
      return this->value;
    }

    const std::string& String() const override
    {
      throw std::bad_cast();
    }

    number_t(double value)
    :value(value)
    {
      ;
    }

    ~number_t() override
    {
      ;
    }
  };

  class string_t final: public bb::value_t
  {
    std::string value;

    string_t(const string_t&) = delete;
    string_t& operator =(const string_t&) = delete;
    string_t(string_t&&) = delete;
    string_t& operator =(string_t&&) = delete;

  public:

    bb::type_t Type() const override
    {
      return bb::type_t::string;
    }

    double Number() const override
    {
      throw std::bad_cast();
    }

    const std::string& String() const override
    {
      return this->value;
    }

    string_t(const std::string& value)
    :value(value)
    {
      ;
    }

    string_t(std::string&& value)
    :value(std::move(value))
    {
      ;
    }

    ~string_t() override
    {
      ;
    }

  };

} // namespace 

namespace bb
{

  value_t::value_t()
  {
    ;
  }

  value_t::~value_t() 
  {
    ;
  }

  ref_t::ref_t(std::unique_ptr<value_t>&& val)
  :val(std::move(val))
  {
    ;
  }

  ref_t ref_t::Number(double value)
  {
    return ref_t(std::unique_ptr<value_t>(new number_t(value)));
  }

  ref_t ref_t::String(const std::string& value)
  {
    return ref_t(std::unique_ptr<value_t>(new string_t(value)));
  }

} // namespace bb
