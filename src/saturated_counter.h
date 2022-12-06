#pragma once
/**
 * @brief A saturated counter given an initial value and a max range
 *
 */
template <typename T>
class saturated_counter {
   // meta bookkeeping
  private:
   using num_t = T;

   // members
  private:
   num_t _range;
   num_t _num;

   // defaults
  public:
   saturated_counter() = default;
   saturated_counter(const saturated_counter &) = default;
   saturated_counter(saturated_counter &&) = default;
   ~saturated_counter() = default;

  public:
   explicit saturated_counter(num_t range, num_t num = 0) : _range{range}, _num{num} {}

   const num_t &num() { return _num; }
   // operators
   // postfix
   saturated_counter operator++(int) {
      if (_num < _range) _num++;
      return *this;
   }
   saturated_counter operator--(int) {
      if (_num > -_range) _num--;
      return *this;
   }
   // equality
   bool operator<(const saturated_counter &other) const { return _num < other._num; }
   bool operator>(const saturated_counter &other) const { return _num > other._num; }
   bool operator==(const saturated_counter &other) const {
      return !(*this > other || *this < other);
   }
};