#pragma once

#include <algorithm>
#include <array>
#include <compare>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <type_traits>

#ifdef __cpp_exceptions
#include <exception>
#define STATIC_VECTOR_THROW(x) throw(x)
#else
#define STATIC_VECTOR_THROW(x) std::abort();
#endif

namespace base {

/**
 * @brief Implementation of standard vector with statically allocated pool
 * The container uses underlying array to allocate required capacity.
 * For detailed documentation see https://en.cppreference.com/w/cpp/container/vector
 * The documentation below highlights only the differences.
 */
template <typename T, std::size_t N> class static_vector {
public:
    using value_type = T;  ///< The type of the elements
    using storage_container_type = std::array<std::aligned_storage_t<sizeof(T), alignof(T)>,
                                              N>;  ///< Type of underlying continuous static storage
    using size_type = typename storage_container_type::size_type;  ///< Unsigned integer type
                                                                   ///< (usually std::size_t)

    using pointer = T*;              ///< Pointer to element type
    using const_pointer = T const*;  ///< Const pointer to element type

    using reference = T&;              ///< Reference to element type
    using const_reference = T const&;  ///< Const reference to element type

    using iterator = pointer;              ///< Iterator to value_type type that meets
                                           ///< LegacyRandomAccessIterator requirements
    using const_iterator = const_pointer;  ///< Iterator to const value_type type that meets
                                           ///< LegacyRandomAccessIterator requirements

    using reverse_iterator = std::reverse_iterator<iterator>;  ///< Reverse iterator type
    using const_reverse_iterator =
            std::reverse_iterator<const_iterator>;  ///< Reverse const iterator type

    using difference_type = decltype(std::distance(
            std::declval<iterator>(),
            std::declval<iterator>()));  ///< Signed integer type (usually std::ptrdiff_t)

private:
    storage_container_type container;  ///< element storage
    size_type currentSize{0};          ///< current element count

public:
    /// Default constructor. Constructs an empty container with a default-constructed allocator
    constexpr static_vector() noexcept = default;

    /**
     * @brief Constructs the container with count copies of elements with value value
     * @param[in] count the size of the container
     * @param[in] value the value to initialize elements of the container with
     * @note The method will throw LengthError if count > max_size()
     */
    constexpr static_vector(size_type count, const_reference value) : currentSize(count) {
        if (count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        std::uninitialized_fill_n(begin(), count, value);
    }

    /**
     * @brief Constructs the container with count default-inserted instances of T. No copies are
     * made
     * @param[in] count the size of the container
     * @note The method will throw LengthError if count > max_size()
     */
    constexpr explicit static_vector(size_type count) : currentSize(count) {
        if (count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        std::uninitialized_default_construct_n(begin(), count);
    }

    /**
     * @brief Constructs the container with the contents of the range [first, last)
     * @param[in] first start of the range to copy the elements from
     * @param[in] last end of the range to copy the elements from
     * @note The method will throw LengthError if std::distance(first, last) is negative or, greater
     * than max_size()
     */
    template <class InputIt> constexpr static_vector(InputIt first, InputIt last) {
        auto dist = std::distance(first, last);
        auto count = size_type(dist);
        if (dist < 0 || count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        currentSize = count;
        std::uninitialized_copy(first, last, begin());
    }

    /**
     * @brief Copy constructor. Constructs the container with the copy of the contents of other
     * @param[in] other another container to be used as source to initialize the elements of the
     * container with
     */
    constexpr static_vector(static_vector const& other) : currentSize(other.currentSize) {
        std::uninitialized_copy(other.begin(), other.end(), begin());
    }

    /**
     * @brief Move constructor. Constructs the container with the contents of other using move
     * semantics. After the move, other is guaranteed to be empty()
     * @param[in] other another container to be used as source to initialize the elements of the
     * container with
     */
    constexpr static_vector(static_vector&& other) noexcept
            : currentSize(std::move(other.currentSize)) {
        std::uninitialized_move(other.begin(), other.end(), begin());
    }

    /**
     * @brief Constructs the container with the contents of the initializer list init
     * @param[in] init initializer list to initialize the elements of the container with
     * @note The method will throw LengthError if init.size() > max_size()
     */
    constexpr static_vector(std::initializer_list<value_type> init)
            : static_vector(init.begin(), init.end()) {}

    /**
     * @brief Destructs the vector. The destructors of the elements are called and the used storage
     * is deallocated.
     * @note if the elements are pointers, the pointed-to objects are not destroyed.
     */
    constexpr ~static_vector() noexcept {
        if constexpr (!std::is_trivially_destructible_v<value_type>) {
            clear();
        }
    }

    /**
     * @brief Copy assignment operator. Replaces the contents with a copy of the contents of other
     * @param[in] other another container to use as data source
     * @return *this
     */
    constexpr static_vector& operator=(static_vector const& other) {
        if (&other != this) {
            if (!empty()) {
                clear();
            }
            currentSize = other.currentSize;
            std::uninitialized_copy(other.begin(), other.end(), begin());
        }
        return *this;
    }

    /**
     * @brief Move assignment operator. Replaces the contents with those of other using move
     * semantics (i.e. the data in other is moved from other into this container). other is in a
     * valid but unspecified state afterwards
     * @param[in] other another container to use as data source
     * @return *this
     */
    constexpr static_vector& operator=(static_vector&& other) {
        if (&other != this) {
            if (!empty()) {
                clear();
            }
            currentSize = std::move(other.currentSize);
            std::uninitialized_move(other.begin(), other.end(), begin());
        }
        return *this;
    }

    /**
     * @brief  Replaces the contents with those identified by initializer list ilist
     * @param[in] ilist initializer list to use as data source
     * @return *this
     * @note The method will throw LengthError if ilist.size() > max_size()
     */
    constexpr static_vector& operator=(std::initializer_list<value_type> ilist) {
        if (!empty()) {
            clear();
        }
        if (ilist.size() > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        currentSize = ilist.size();
        std::uninitialized_copy(ilist.begin(), ilist.end(), begin());
        return *this;
    }

    /**
     * @brief Replaces the contents with count copies of value value
     * @param[in] count the new size of the container
     * @param[in] value the value to initialize elements of the container with
     * @note The method will throw LengthError if count > max_size()
     */
    constexpr void assign(size_type count, const T& value) {
        if (!empty()) {
            clear();
        }
        if (count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        std::uninitialized_fill_n(begin(), count, value);
        currentSize = count;
    }

    /**
     * @brief Replaces the contents with copies of those in the range [first, last)
     * @param[in] first start of the range to copy the elements from
     * @param[in] last end of the range to copy the elements from
     * @note  The behavior is undefined if either argument is an iterator into *this.
     * @note The method will throw LengthError if std::distance(first, last) is negative or, greater
     * than max_size()
     */
    template <class InputIt> constexpr void assign(InputIt first, InputIt last) {
        if (!empty()) {
            clear();
        }

        auto dist = std::distance(first, last);
        auto count = size_type(dist);
        if (dist < 0 || count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        currentSize = count;
        std::uninitialized_copy(first, last, begin());
    }

    /**
     * @brief Replaces the contents with the elements from the initializer list ilist
     * @param[in] ilist initializer list to copy the values from
     * @note The method will throw LengthError if ilist.size() > max_size()
     */
    constexpr void assign(std::initializer_list<T> ilist) { assign(ilist.begin(), ilist.end()); }

    /**
     * @brief Returns a reference to the element at specified location pos, with bounds checking.
     * @param[in] pos position of the element to return
     * @return Reference to the requested element
     * @note If pos is not within the range of the container, an OutOfRangeError is thrown.
     */
    constexpr reference at(size_type pos) {
        if (pos >= size()) {
            STATIC_VECTOR_THROW(std::out_of_range("Index out of bounds"));
        }
        return (*this)[pos];
    }

    /**
     * @brief Returns a reference to the element at specified location pos, with bounds checking.
     * @param[in] pos 	position of the element to return
     * @return Reference to the requested element
     * @note If pos is not within the range of the container, an OutOfRangeError is thrown.
     */
    constexpr const_reference at(size_type pos) const {
        if (pos >= size()) {
            STATIC_VECTOR_THROW(std::out_of_range("Index out of bounds"));
        }
        return (*this)[pos];
    }

    /**
     * @brief Returns a reference to the element at specified location pos. No bounds checking is
     * performed
     * @param[in] pos position of the element to return
     * @return Reference to the requested element
     * @note Accessing a nonexistent element through this operator is undefined behavior
     */
    constexpr reference operator[](size_type pos) noexcept { return *(begin() + pos); }

    /**
     * @brief Returns a reference to the element at specified location pos. No bounds checking is
     * performed
     * @param[in] pos position of the element to return
     * @return Reference to the requested element
     * @note Accessing a nonexistent element through this operator is undefined behavior
     */
    constexpr const_reference operator[](size_type pos) const noexcept { return *(begin() + pos); }

    /**
     * @brief Returns a reference to the first element in the container
     * @return Reference to the first element
     * @note Calling front on an empty container is undefined
     */
    constexpr reference front() noexcept { return *begin(); }

    /**
     * @brief Returns a reference to the first element in the container
     * @return Reference to the first element
     * @note Calling front on an empty container is undefined
     */
    constexpr const_reference front() const noexcept { return *begin(); }

    /**
     * @brief Returns a reference to the last element in the container
     * @return Reference to the last element
     * @note Calling back on an empty container is undefined
     */
    constexpr reference back() noexcept { return *(end() - 1); }

    /**
     * @brief Returns a reference to the last element in the container
     * @return Reference to the last element
     * @note Calling back on an empty container is undefined
     */
    constexpr const_reference back() const noexcept { return *(end() - 1); }

    /**
     * @brief Returns pointer to the underlying array serving as element storage
     * @return Pointer to the underlying element storage. For non-empty containers, the returned
     * pointer compares equal to the address of the first element
     * @note The pointer is such that range [data(); data() + size()) is always a valid range, even
     * if the container is empty (data() is not dereferenceable in that case)
     */
    constexpr pointer data() noexcept {
        return std::launder(reinterpret_cast<pointer>(container.begin()));
    }

    /**
     * @brief Returns pointer to the underlying array serving as element storage
     * @return Pointer to the underlying element storage. For non-empty containers, the returned
     * pointer compares equal to the address of the first element
     * @note The pointer is such that range [data(); data() + size()) is always a valid range, even
     * if the container is empty (data() is not dereferenceable in that case)
     */
    constexpr const_pointer data() const noexcept {
        return std::launder(reinterpret_cast<const_pointer>(container.begin()));
    }

    /**
     * @brief Returns an iterator to the first element of the vector.
     * If the vector is empty, the returned iterator will be equal to end()
     * @return Iterator to the first element
     */
    constexpr iterator begin() noexcept { return data(); }

    /**
     * @brief Returns an iterator to the first element of the vector.
     * If the vector is empty, the returned iterator will be equal to end()
     * @return Iterator to the first element
     */
    constexpr const_iterator begin() const noexcept { return data(); }

    /**
     * @brief Returns an iterator to the first element of the vector.
     * If the vector is empty, the returned iterator will be equal to end()
     * @return Iterator to the first element
     */
    constexpr const_iterator cbegin() const noexcept { return data(); }

    /**
     * @brief Returns an iterator to the element following the last element of the vector
     * @return Iterator to the element following the last element
     * @note This element acts as a placeholder; attempting to access it results in undefined
     * behavior.
     */
    constexpr iterator end() noexcept { return begin() + currentSize; }

    /**
     * @brief Returns an iterator to the element following the last element of the vector
     * @return Iterator to the element following the last element
     * @note This element acts as a placeholder; attempting to access it results in undefined
     * behavior.
     */
    constexpr const_iterator end() const noexcept { return begin() + currentSize; }

    /**
     * @brief Returns an iterator to the element following the last element of the vector
     * @return Iterator to the element following the last element
     * @note This element acts as a placeholder; attempting to access it results in undefined
     * behavior.
     */
    constexpr const_iterator cend() const noexcept { return begin() + currentSize; }

    /**
     * @brief Returns a reverse iterator to the first element of the reversed vector.
     * It corresponds to the last element of the non-reversed vector. If the vector is empty, the
     * returned iterator is equal to rend().
     * @return Reverse iterator to the first element
     */
    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

    /**
     * @brief Returns a reverse iterator to the first element of the reversed vector.
     * It corresponds to the last element of the non-reversed vector. If the vector is empty, the
     * returned iterator is equal to rend().
     * @return Reverse iterator to the first element
     */
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator(end());
    }

    /**
     * @brief Returns a reverse iterator to the first element of the reversed vector.
     * It corresponds to the last element of the non-reversed vector. If the vector is empty, the
     * returned iterator is equal to rend().
     * @return Reverse iterator to the first element
     */
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator(cend());
    }

    /**
     * @brief Returns a reverse iterator to the element following the last element of the reversed
     * vector. It corresponds to the element preceding the first element of the non-reversed vector
     * @return Reverse iterator to the element following the last element
     * @note This element acts as a placeholder, attempting to access it results in undefined
     * behavior
     */
    constexpr reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

    /**
     * @brief Returns a reverse iterator to the element following the last element of the reversed
     * vector. It corresponds to the element preceding the first element of the non-reversed vector
     * @return Reverse iterator to the element following the last element
     * @note This element acts as a placeholder, attempting to access it results in undefined
     * behavior
     */
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator(begin());
    }

    /**
     * @brief Returns a reverse iterator to the element following the last element of the reversed
     * vector. It corresponds to the element preceding the first element of the non-reversed vector
     * @return Reverse iterator to the element following the last element
     * @note This element acts as a placeholder, attempting to access it results in undefined
     * behavior
     */
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator(begin());
    }

    /**
     * @brief Checks if the container has no elements, i.e. whether begin() == end()
     * @return true if the container is empty, false otherwise
     */
    constexpr bool empty() const noexcept { return 0 == size(); }

    /**
     * @brief Returns the number of elements in the container, i.e. std::distance(begin(), end())
     * @return The number of elements in the container
     */
    constexpr size_type size() const noexcept { return currentSize; }

    /**
     * @brief Returns the maximum number of elements the container is able to hold due to system or
     * library implementation limitations i.e. N
     * @return Maximum number of elements
     */
    constexpr size_type max_size() const noexcept { return container.max_size(); }

    /**
     * @brief Increase the capacity of the vector to a value that's greater or equal to new_cap.
     * Because the static_vector uses static storage, when new_cap <= max_size() the method does
     * nothing.
     * @param[in] new_cap 	new capacity of the vector
     * @note For compatibility purposes, The method throws LengthError when new_cap > max_size
     */
    constexpr void reserve(size_type new_cap) {
        if (new_cap > max_size()) {
            STATIC_VECTOR_THROW(std::length_error("Reserve exceeds max_size"));
        }
    }

    /**
     * @brief Returns the number of elements that the container has currently allocated space for
     * @return Capacity of the currently allocated storage
     */
    constexpr size_type capacity() const noexcept { return max_size(); }

    /**
     * @brief Requests the removal of unused capacity.
     * Because the static_vector uses static storage this method does nothing
     */
    constexpr void
    shrink_to_fit() noexcept { /* Do nothing, as the underlying storage cannot be shrinked */
    }

    /**
     * @brief Erases all elements from the container. After this call, size() returns zero.
     * Invalidates any references, pointers, or iterators referring to contained elements. Any
     * past-the-end iterators are also invalidated. Leaves the capacity() of the vector unchanged
     */
    constexpr void clear() noexcept {
        std::for_each(begin(), end(), [](reference x) { std::destroy_at(&x); });
        currentSize = 0;
    }

    /**
     * @brief Inserts value before pos
     * @param[in] pos iterator before which the content will be inserted. pos may be the end()
     * iterator
     * @param[in] value element value to insert
     * @return Iterator pointing to the inserted value
     * @note The method will throw LengthError if size() == max_size()
     */
    constexpr iterator insert(const_iterator pos, T const& value) {
        if (size() + 1 > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        auto position = begin() + std::distance(cbegin(), pos);
        // Move last element right by one (end() + 1 will become new end(), so uninitialized memory
        // need to be initialized)
        if (position != end()) {
            std::uninitialized_move(end() - 1, end(), end());
            // Move [pos, end() -1) to [pos + 1, end())
            std::move_backward(position, end() - 1, end());
        }
        std::construct_at(position, value);
        ++currentSize;
        return position;
    }

    /**
     * @brief Inserts value before pos
     * @param[in] pos iterator before which the content will be inserted. pos may be the end()
     * iterator
     * @param[in] value element value to insert
     * @return Iterator pointing to the inserted value
     * @note The method will throw LengthError if size() == max_size()
     */
    constexpr iterator insert(const_iterator pos, T&& value) {
        if (size() + 1 > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        auto position = begin() + std::distance(cbegin(), pos);
        // Move last element right by one (end() + 1 will become new end(), so uninitialized memory
        // need to be initialized)
        if (position != end()) {
            std::uninitialized_move(end() - 1, end(), end());
            // Move [pos, end() -1) to [pos + 1, end())
            std::move_backward(position, end() - 1, end());
        }
        std::construct_at(position, std::move(value));
        ++currentSize;
        return position;
    }

    /**
     * @brief  Inserts count copies of the value before pos
     * @param[in] pos iterator before which the content will be inserted. pos may be the end()
     * iterator
     * @param[in] count number of copies to be inserted
     * @param[in] value element value to insert
     * @return Iterator pointing to the first element inserted, or pos if count==0
     * @note The method will throw LengthError if size() + count > max_size()
     */
    constexpr iterator insert(const_iterator pos, size_type count, T const& value) {
        if (size() + count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        auto position = begin() + std::distance(cbegin(), pos);
        if (count > 0) {
            if (position != end()) {
                auto existingElementsToMove = std::distance(
                        position, end());  // Negative distance in this context is UB (position must
                                           // be in range [begin(), end()])
                auto toCopyAtTheEnd =
                        (count >= existingElementsToMove) ? (count - existingElementsToMove) : 0;
                auto toMoveAssign =
                        (count >= existingElementsToMove) ? 0 : (existingElementsToMove - count);

                // uninitialized_copy last toCopyAtTheEnd elements of input range at the end(), as
                // they don't overlap with existing data
                auto lastElem = std::uninitialized_fill_n(end(), toCopyAtTheEnd, value);

                // Move data from [pos end()) after last element of the vector. If size of the input
                // range is smaller than number of elements to move
                std::uninitialized_move(position + toMoveAssign, end(), lastElem);

                std::move_backward(position, position + toMoveAssign, end());

                std::fill(position, position + count - toCopyAtTheEnd, value);

            } else {
                std::uninitialized_fill_n(position, count, value);
            }
            currentSize += count;
        }
        return position;
    }

    /**
     * @brief Inserts elements from range [first, last) before pos
     * @param[in] pos iterator before which the content will be inserted. pos may be the end()
     * iterator
     * @param[in] first start of the range of elements to insert, can't be iterators into container
     * for which insert is called
     * @param[in] last end of the range of elements to insert, can't be iterators into container for
     * which insert is called
     * @return Iterator pointing to the first element inserted, or pos if first==last
     * @note The method will throw LengthError if size() + count > max_size()
     */
    template <class InputIt>
    constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
        auto dist = std::distance(first, last);
        auto count = size_type(dist);
        if (dist < 0 || (size() + count > max_size())) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        auto position = begin() + std::distance(cbegin(), pos);
        if (position != end()) {
            auto existingElementsToMove =
                    std::distance(position, end());  // Negative distance in this context is UB
                                                     // (position must be in range [begin(), end()])
            auto toCopyAtTheEnd =
                    (dist >= existingElementsToMove) ? (dist - existingElementsToMove) : 0;
            auto toMoveAssign =
                    (dist >= existingElementsToMove) ? 0 : (existingElementsToMove - dist);

            // uninitialized_copy last toCopyAtTheEnd elements of input range at the end(), as they
            // don't overlap with existing data
            auto lastElem = std::uninitialized_copy(last - toCopyAtTheEnd, last, end());

            // Move data from [pos end()) after last element of the vector. If size of the input
            // range is smaller than number of elements to move
            //
            std::uninitialized_move(position + toMoveAssign, end(), lastElem);

            std::move_backward(position, position + toMoveAssign, end());

            std::copy(first, last - toCopyAtTheEnd, position);
        } else {
            std::uninitialized_copy(first, last, end());
        }
        currentSize += count;
        return position;
    }

    /**
     * @brief Inserts elements from initializer list ilist before pos
     * @param[in] pos iterator before which the content will be inserted. pos may be the end()
     * iterator
     * @param[in] ilist initializer list to insert the values from
     * @return Iterator pointing to the first element inserted, or pos if ilist is empty
     * @note The method will throw LengthError if size() + ilist.size() > max_size()
     */
    constexpr iterator insert(const_iterator pos, std::initializer_list<T> ilist) {
        return insert(pos, ilist.begin(), ilist.end());
    }

    /**
     * @brief Inserts a new element into the container directly before pos
     * @param pos iterator before which the new element will be constructed
     * @param args arguments to forward to the constructor of the element
     * @return Iterator pointing to the emplaced element
     * @note The method will throw LengthError if size() == max_size()
     */
    template <class... Args> constexpr iterator emplace(const_iterator pos, Args&&... args) {
        if (size() + 1 > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        auto const position = begin() + std::distance(cbegin(), pos);
        // Move last element right by one (end() + 1 will become new end(), so uninitialized memory
        // need to be initialized)
        if (position != end()) {
            std::uninitialized_move(end() - 1, end(), end());
            // Move [pos, end() -1) to [pos + 1, end())
            std::move_backward(position, end() - 1, end());
        } else {
            std::construct_at(position, std::forward<Args>(args)...);
        }
        ++currentSize;
        return position;
    }

    /**
     * @brief Removes the element at pos
     * @param[in] pos to the element to remove
     * @return Iterator following the last removed element
     * @note If pos refers to the last element, then the end() iterator is returned
     */
    constexpr iterator erase(const_iterator pos) {
        auto index = std::distance(cbegin(), pos);
        std::move(begin() + index + 1, end(), begin() + index);
        // Elements were moved left, now destroy the last element
        currentSize--;
        // Now, end() points to previous last element
        std::destroy_at(end());
        return begin() + index;
    }

    /**
     * @brief Removes the elements in the range [first, last)
     * @param[in] first start of range of elements to remove
     * @param[in] last end of range of elements to remove
     * @return Iterator following the last removed element
     * @note If last==end() prior to removal, then the updated end() iterator is returned.
     * @note If [first, last) is an empty range, then last is returned
     */
    constexpr iterator erase(const_iterator first, const_iterator last) {
        auto last_ = begin() + std::distance(cbegin(), last);
        if (first >= last) return last_;
        auto first_ = begin() + std::distance(cbegin(), first);

        auto toErase = std::distance(first, last);  // Guaranteed to be > 0
        auto lastValid = std::move(first_ + toErase, end(), first_);
        std::for_each(lastValid, end(), [](reference x) { std::destroy_at(&x); });

        currentSize -= size_type(toErase);
        return first_;
    }

    /**
     * @brief Appends the given element value to the end of the container. The new element is
     * initialized as a copy of value
     * @param[in] value	the value of the element to append
     * @note The method will throw LengthError if size() == max_size()
     */
    constexpr void push_back(T const& value) {
        if (size() + 1 > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        std::construct_at(end(), value);
        ++currentSize;
    }

    /**
     * @brief Appends the given element value to the end of the container; value is moved into the
     * new element
     * @param[in] value	the value of the element to append
     * @note The method will throw LengthError if size() == max_size()
     */
    constexpr void push_back(T&& value) {
        if (size() + 1 > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        std::construct_at(end(), std::move(value));
        ++currentSize;
    }

    /**
     * @brief Appends a new element to the end of the container. The element is constructed
     * in-place.
     * @param[in] args arguments to forward to the constructor of the element
     * @return A reference to the inserted element
     * @note The method will throw LengthError if size() == max_size()
     */
    template <class... Args> constexpr reference emplace_back(Args&&... args) {
        if (size() + 1 > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        }
        auto const position = end();
        std::construct_at(position, std::forward<Args>(args)...);
        ++currentSize;
        return *position;
    }

    /**
     * @brief Removes the last element of the container
     * Iterators and references to the last element, as well as the end() iterator, are invalidated
     * @note Calling pop_back on an empty container results in undefined behavior.
     */
    constexpr void pop_back() {
        currentSize--;
        std::destroy_at(end());
    }

    /**
     * @brief Resizes the container to contain count elements
     * If the current size is greater than count, the container is reduced to its first count
     * elements. If the current size is less than count, additional default-inserted elements are
     * appended
     * @param[in] count new size of the container
     * @note The method will throw LengthError if count > max_size()
     */
    constexpr void resize(size_type count) {
        if (count < size()) {
            erase(begin() + count, end());
        } else if (count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        } else {
            auto toAdd = count - size();
            for (size_type i = 0; i != toAdd; ++i) {
                emplace_back();
            }
        }
    }

    /**
     * @brief Resizes the container to contain count elements
     * If the current size is greater than count, the container is reduced to its first count
     * elements. If the current size is less than count,  additional copies of value are appended.
     * @param[in] count new size of the container
     * @param[in] value the value to initialize the new elements with
     * @note The method will throw LengthError if count > max_size()
     */
    constexpr void resize(size_type count, value_type const& value) {
        if (count < size()) {
            erase(begin() + count, end());
        } else if (count > max_size()) {
            STATIC_VECTOR_THROW(
                    std::length_error("Insertion would exceed static_vector capacity"));
        } else {
            auto toAdd = count - size();
            for (size_type i = 0; i != toAdd; ++i) {
                push_back(value);
            }
        }
    }

    /**
     * Exchanges the contents of the container with those of other. Swaps content of underlying
     * storage. All iterators and references are invalidated
     * @param[in] other reference to static_vector instance to swap with
     */
    constexpr void swap(static_vector& other) noexcept {
        container.swap(other.container);
        std::swap(currentSize, other.currentSize);
    }
};

/**
 * @brief Checks if the contents of lhs and rhs are equal, that is, they have the same number of
 * elements and each element in lhs compares equal with the element in rhs at the same position
 * @param[in] lhs first of vectors whose contents to compare
 * @param[in] rhs second of vectors whose contents to compare
 * @return true if the contents of the vectors are equal, false otherwise
 */
template <class T, std::size_t N>
constexpr bool operator==(static_vector<T, N> const& lhs, static_vector<T, N> const& rhs) {
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/**
 * @brief Compares the contents of lhs and rhs lexicographically.
 * The comparison is performed as if by calling std::lexicographical_compare_three_way on two
 * vectors with a function object performing synthesized three-way comparison. The return type is
 * same as the result type of synthesized three-way comparison
 * @param[in] lhs first of vectors whose contents to compare
 * @param[in] rhs second of vectors whose contents to compare
 * @return The relative order of the first pair of non-equivalent elements in lhs and rhs if there
 * are such elements, lhs.size() <=> rhs.size() otherwise
 */
template <class T, std::size_t N>
constexpr auto operator<=>(static_vector<T, N> const& lhs, static_vector<T, N> const& rhs) {
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

/**
 * @brief Specializes the std::swap algorithm for static_vector.
 * Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
 * @param[in,out] lhs container whose contents to swap
 * @param[in,out] rhs containers whose contents to swap
 */
template <typename T, std::size_t N>
constexpr void swap(static_vector<T, N>& lhs, static_vector<T, N>& rhs) noexcept {
    lhs.swap(rhs);
}

/**
 * @brief Erases all elements that compare equal to value from the container
 * @param[in, out] c container from which to erase
 * @param[in] value value to be removed
 * @return The number of erased elements
 */
template <typename T, std::size_t N, typename U>
constexpr typename static_vector<T, N>::size_type erase(static_vector<T, N>& c, U const& value) {
    auto oldSize = c.size();
    auto end = std::remove(c.begin(), c.end(), value);
    c.erase(end, c.end());
    return c.size() - oldSize;
}

/**
 * @brief Erases all elements that compare equal to value from the container
 * @param[in, out] c container from which to erase
 * @param[in] pred unary predicate which returns ​true if the element should be erased
 * @return The number of erased elements
 */
template <typename T, std::size_t N, typename Pred>
constexpr typename static_vector<T, N>::size_type erase_if(static_vector<T, N>& c, Pred pred) {
    auto oldSize = c.size();
    auto end = std::remove_if(c.begin(), c.end(), pred);
    c.erase(end, c.end());
    return c.size() - oldSize;
}
}  // namespace base
