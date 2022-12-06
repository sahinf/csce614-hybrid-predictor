#pragma once
/**
 * @brief A saturated counter given an initial value and a max range
 *
 */
template <typename T>
class saturated_counter {
   // members
  private:
   T _range;
   T _num;

   // defaults
  public:
   // saturated_counter() = default;
   // saturated_counter(const saturated_counter &) = default;
   // saturated_counter(saturated_counter &&) = default;
   // ~saturated_counter() = default;

  public:
   explicit saturated_counter(T range, T num = 0) {
	_range = range;
	_num = num;
   }

   const T &num() { return _num; }
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
