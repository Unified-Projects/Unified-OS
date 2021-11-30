#ifndef __UNIFIED_OS_COMMON_VECTOR_H
#define __UNIFIED_OS_COMMON_VECTOR_H

#include <common/cstring.h>
#include <memory/memory.h>
#include <memory/heap.h>
#include <common/move.h>
#include <common/move.h>
#include <common/ttraits.h>

#include <IO/spinlock.h>

//Class creation
template <typename T> class Vector {

    //Allows iteration through the vector
    class VectorIterator{
        friend class Vector;
    protected:
        //Positioning
        size_t pos = 0;

        //Actual vector
        const Vector<T>& vector;
    public:
        //Basic constructors
        VectorIterator(const Vector<T>& newVector) : vector(newVector){}
        VectorIterator(const VectorIterator& it) : vector(it.vector) {pos = it.pos;}

        //Increase the position
        VectorIterator& operator++(){
            pos++;
            return *this;
        }

        //Increase position
        VectorIterator& operator++(int){
            auto it = *this;

            pos++;
            return it;
        }

        //Reassign another iterater to this
        VectorIterator& operator=(const VectorIterator& other) {
            VectorIterator(other.vector);

            pos = other.pos;

            return *this;
        }

        //Get the data at the iterators position
        inline T& operator*() const { return vector.data[pos]; }

        //Get the data at the iterators position
        inline T* operator->() const { return &vector.data[pos]; }

        //Bools (Usefull for checkinf if we have reached the end)
        inline friend bool operator==(const VectorIterator& l, const VectorIterator& r) {
            return l.pos == r.pos;
        }

        //Bools (Usefull for checkinf if we have reached the end)
        inline friend bool operator!=(const VectorIterator& l, const VectorIterator& r) {
            return l.pos != r.pos;
        }
    };

private:
    //Data
    T* data = nullptr;

    //Sizing
    size_t count = 0;
    size_t capacity = 0;

    //Spinlock
    lock_t lock = 0;

public:

    //Setup
    Vector() = default;

    //Using another vectors data
    Vector(const Vector<T>& x) {
        //Resize
        EnsureCapacity(x.get_length());

        //Move data
        for (unsigned i = 0; i < x.get_length(); i++) {
            data[i] = x.data[i];
        }
    }

    //If we use = {Data...} it will add them all back
    template <typename... D> Vector(D... data) { (add_back(data), ...); }

    //Get the data at a position (similar to [])
    inline T& at(size_t pos) {
        // assert(pos < count); //Implement assert (Halt if condition is false)

        return data[pos];
    }
    inline const T& at(size_t pos) const {
        // assert(pos < count); //Implement assert (Halt if condition is false)

        return data[pos];
    }

    //Get data at postition (Same as at)
    inline const T& get_at(size_t pos) const { return at(pos); }

    //Indexing
    inline T& operator[](size_t pos) { return at(pos); }
    inline const T& operator[](size_t pos) const { return at(pos); }

    //Get the size
    inline size_t size() const { return count; }

    //Get the size (unminified)
    inline size_t get_length() const { return count; }

    //Remove a peice of data at that pos
    void erase(unsigned pos) {
        //Memory lock
        acquireLock(&lock);

        //Erase
        EraseUnlocked(pos);

        releaseLock(&lock);
    }

    //Presize the vector (Without data)
    void reserve(size_t allocatedSize) {
        acquireLock(&lock);

        //Resize
        EnsureCapacity(allocatedSize);

        releaseLock(&lock);
    }
    
    //Resize (Will remove data)
    void resize(size_t newSize) {
        acquireLock(&lock);

        //Ensure size (If we go bigger new data is mallocated)
        EnsureCapacity(newSize);

        //Size diffference
        size_t oldCount = count;
        count = newSize;
        
        //Fill with blank data
        if (count > oldCount) {
            for (unsigned i = oldCount; i < count; i++) {
                new (&data[i]) T();
            }
        } else if (count < oldCount) { //Remove old data
            if constexpr (!TTraits<T>::is_trivial()) {
                for (unsigned i = count; i < oldCount; i++) {
                    data[i].~T();
                }
            }
        }

        releaseLock(&lock);
    }

    //Adds to the end
    T& add_back(const T& val) {
        
        acquireLock(&lock);

        //Resize
        EnsureCapacity(count + 1);

        //Create a reference point to the data
        T& ref = data[count];
        count++;

        //Copy the data to the position
        new (&ref) T(val);
        releaseLock(&lock);

        return ref;
    }

    //Same as ^
    T& add_back(T&& val) {
        acquireLock(&lock);

        EnsureCapacity(count + 1);

        T& ref = data[count];
        count++;

        new (&ref) T(static_cast<T&&>(val));
        releaseLock(&lock);

        return ref;
    }

    //Removes the last value (No actual resizing tho)
    T& pop_back() {
        // assert(count);

        return data[--count];
    }

    //Gets the data
    inline T* Data() { return data; }

    //Remove using a reference instead of a pos
    void remove(const T& val) {
        acquireLock(&lock);

        //Try find the position
        for (unsigned i = 0; i < count; i++) {
            if (data[i] == val) {
                //Remove when found
                EraseUnlocked(i);
                return;
            }
        }

        releaseLock(&lock);
    }

    //Remove all data
    ~Vector() {
        if (data) {
            UnifiedOS::Memory::free(data);
        }
        data = nullptr;
    }

    //Returns a iterator for the beggining position
    VectorIterator begin() const {
        VectorIterator it = VectorIterator(*this);

        it.pos = 0;

        return it;
    }

    //Returns a iterator for the end of the data
    VectorIterator end() const {
        VectorIterator it = VectorIterator(*this);

        it.pos = count;

        return it;
    }  

    //Remove all data
    void clear() {
        acquireLock(&lock);
        if (data) {
            UnifiedOS::Memory::free(data);
        }

        //Default variables
        count = capacity = 0;
        data = nullptr;
        releaseLock(&lock);
    }
private:
    //Resize when nessasary
    inline void EnsureCapacity(unsigned size) {
        //Check if needed
        if (size >= capacity) {
            //New size (at the end)
            size_t newCapacity = capacity + (size << 1) + 1;

            //If we currently contain data
            if (data) {
                //Get the old data
                T* oldData = data;

                //Create a new data
                data = reinterpret_cast<T*>(UnifiedOS::Memory::malloc(newCapacity * sizeof(T)));

                //If new data is needed to be created
                if constexpr (TTraits<T>::is_trivial()) {
                    UnifiedOS::Memory::memcpy(data, oldData, capacity * sizeof(T));
                    UnifiedOS::Memory::memset(data + capacity, 0, sizeof(T) * (size - capacity)); 
                } else {
                    //Swap old data into the new data then clear the old
                    for (unsigned i = 0; i < count && i < capacity; i++) {
                        new (&data[i]) T(move(oldData[i]));
                        oldData[i].~T();
                    }

                    for (unsigned i = capacity; i < size; i++) {
                        new (&data[i]) T();
                    }
                }

                //Remove all the data at the old data
                UnifiedOS::Memory::free(oldData);
            } else {
                //Otherwise just create a data point
                data = reinterpret_cast<T*>(UnifiedOS::Memory::malloc(newCapacity * sizeof(T)));
            }

            capacity = newCapacity;
        }
    }

    //Remove at a position
    inline void EraseUnlocked(unsigned pos) {
        // assert(pos < count);

        //Check if data is next to the end
        if constexpr (TTraits<T>::is_trivial()) {
            UnifiedOS::Memory::memcpy(&data[pos], &data[pos + 1], (count - pos - 1) * sizeof(T));
        } else {
            //Move old data if needed
            for (unsigned i = pos; i < count - 1; i++) {
                data[i] = move(data[i + 1]);
            }
        }

        count--;
    }
};

#endif