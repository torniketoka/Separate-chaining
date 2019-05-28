#ifndef ADS_SET_H
#define ADS_SET_H

#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>
#include <stdexcept>


template <typename Key, size_t N = 7>
class ADS_set {
public:
    class Iterator;
    using value_type = Key;
    using key_type = Key;
    using reference = key_type&;
    using const_reference = const key_type&;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using iterator = Iterator;
    using const_iterator = Iterator;
    using key_compare = std::less<key_type>;   // B+-Tree
    using key_equal = std::equal_to<key_type>; // Hashing
    using hasher = std::hash<key_type>;        // Hashing
    
private:
    
    
    struct Bucket
    {
        Bucket* next = nullptr;
        key_type value;
        
        ~Bucket()
        {
            delete next;
        }
    };
    
    size_t sz=0;
    size_t max_sz{N};
    Bucket* table{nullptr};
    
    
    size_type hash_idx(const key_type& k) const { return hasher{}(k) % max_sz;}
    
    
    
    bool find_element(const key_type& key) const
    {
        size_type a = hash_idx(key);
        if(table[a].next==nullptr) return false;
        for(Bucket* i = table[a].next; i != nullptr; i = i -> next)
        {
            if(key_equal{}(key, i->value))
            {
                return true;
            }
        }
        return false;
    }

    void rehash () 
    {
        Key* bucket = new Key[sz];
        size_t zahl{0};
        for(const auto& it : *this)
        bucket[zahl++]=it;

        delete[] table;
        max_sz = max_sz*2;
        table = new Bucket[max_sz];
        sz=0;
        for(size_t i = 0;  i < zahl; i++)
        insert_unchecked(bucket[i]);
        delete [] bucket; 
    }

   void insert_unchecked (const key_type& key)
    {
        Bucket* help = new Bucket();
        size_type position = hash_idx(key);
        help -> next = table[position].next;
        help -> value = key;
        table[position].next = help;
        sz++;
        if(sz > max_sz*5) rehash();
    }
      
   
    
    
public:
    
    ADS_set()
    {
        table = new Bucket[max_sz];
    }

    
    ADS_set(std::initializer_list<key_type> ilist)
    {
        table = new Bucket[max_sz];
        insert(ilist);
    }
    
    
    template<typename InputIt> ADS_set(InputIt first, InputIt last)
    {
        table=new Bucket[max_sz];
        insert(first, last);
    }
    
    
    
    ADS_set(const ADS_set& other)
    {
        table = new Bucket[max_sz];
        for(const auto& elem:other) 
        insert_unchecked(elem);
    }
    
    ~ADS_set()
    {
        delete [] table;
    }
    
    
    ADS_set& operator=(const ADS_set& other)
    {
        clear();
        for(const auto& elem: other) 
        insert_unchecked(elem);
        return *this;
    }
    
    
    ADS_set& operator=(std::initializer_list<key_type> ilist)
    {
        delete [] table;
        sz=0;
        table = new Bucket[max_sz];
        insert(ilist);
        return *this;
    }
    
    
    size_type size() const
    {
        return sz;
    }
    
    
    bool empty() const
    {
        return sz == 0 ? true: false;
    }
    
    
    size_type count(const key_type& key) const
    {
        return find_element(key);
    }
    
    
    iterator find(const key_type& key) const
    {
        size_type a = hash_idx(key);
        if(table[a].next==nullptr) return const_iterator{nullptr};
        for(Bucket* i = table[a].next; i != nullptr; i = i -> next)
        {
            if(key_equal{}(key, i->value))
            {
                return const_iterator{i, table, max_sz, a};
            }
        }
        return const_iterator{nullptr};
    }
    
    
    void clear()
    {
        delete [] table;
        sz = 0;   
        table = new Bucket[max_sz];
    }
    
    
    void swap(ADS_set& other)
    {
        using std::swap;
        swap(sz, other.sz);
        swap(max_sz, other.max_sz);
        swap(table, other.table);
    }
   
    
    void insert(std::initializer_list<key_type> ilist)
    {
        for(auto& a:ilist )
        {
            if(!find_element(a))
            insert_unchecked(a);
        }
    }
    
    
   
    
    std::pair<iterator,bool> insert(const key_type& key)
    {
        if(find_element(key))
            return std::make_pair(find(key),  false);
        else
        {
            insert_unchecked(key);
        }
        return std::make_pair(find(key), true);
    }
    
    template<typename InputIt> void insert(InputIt first, InputIt last)
    {
        for (auto it = first; it != last; it++)
        {
            if (!find_element(*it))
            {
                insert_unchecked(*it);
            }
        }
    }
    
    
    
    size_type erase(const key_type& key)
    {
        if(find_element(key))
        {
           size_type ellement = hash_idx(key);
           Bucket* help=table[ellement].next;
           Bucket* helpplz=nullptr;
           if(key_equal{}(help->value, key))
           {
               table[ellement].next=help->next;
               help->next=nullptr;
               delete help;
               sz--;
               return 1;
           }  else
              {
                for(Bucket* i = table[ellement].next->next; i != nullptr; i = i -> next)
                {
                   helpplz=help;
                   help=help->next;
                   if(key_equal{}(help->value, key))
                   {
                      helpplz->next=help->next;
                      help->next=nullptr;
                      helpplz=nullptr;
                      delete helpplz;
                      delete help;
                      sz--;
                      return 1;
                   }
                }
                    
             }
            
        }
        return 0;
    }
    
    
    const_iterator begin() const
    {
        for(size_type i = 0; i < max_sz; ++i)
        {
            if(table[i].next!= nullptr)
            return const_iterator{table[i].next, table, max_sz, i};
        }
        return const_iterator{nullptr};
    }
    
    
    const_iterator end() const
    {
        return const_iterator{nullptr};
    }
    
    
    
    void dump(std::ostream& o = std::cerr) const
    {
        for(size_t zahl{0}; zahl<max_sz; zahl++)
        {
            if(table[zahl].next==nullptr)continue;
            o << "Block|"<<zahl<<"|: ";
            for(Bucket* i=table[zahl].next; i!= nullptr; i=i -> next)
            {
                o << i->value;
                if(i -> next != nullptr) o << "-->";
            }
            o<<'\n';
        }
    }
    
    friend bool operator==(const ADS_set& lhs, const ADS_set& rhs)
    {
        if(lhs.sz != rhs.sz) return false;
        for(const auto& elem:lhs)
        if(!rhs.find_element(elem)) return false;
        return true;
    }
    
    
    
    friend bool operator!=(const ADS_set& lhs, const ADS_set& rhs)
    {
        return !(lhs==rhs);
    }
    
    
  
};


template <typename Key, size_t N>
class ADS_set<Key,N>::Iterator {
    
    Bucket* position;
    Bucket* table;
    size_t max_sz;
    size_t ellement;
    
public:
    using value_type = Key;
    using difference_type = std::ptrdiff_t;
    using reference = const value_type&;
    using pointer = const value_type*;
    using iterator_category = std::forward_iterator_tag;
    
    explicit Iterator(Bucket* position=nullptr, Bucket* table=nullptr, size_t max_sz=0, size_t ellement = 0):
    position{position}, table{table}, max_sz{max_sz}, ellement{ellement} {}
    
    ~Iterator()
    {
        position=nullptr;
        table=nullptr;
        delete position;
        delete [] table;
    }
    
    
    reference operator*() const 
    {
      return position->value;
    }

    pointer operator->() const 
    {
      return &position->value;
    }
    
    
    
    Iterator& operator++()
    {
        if(position->next!=nullptr)
        {
            position = position->next;
        }  else
         {
            ellement++;
            while(table[ellement].next==nullptr && ellement < max_sz)
            {
                ellement++;
            }
            if(ellement == max_sz)
            {
                position = nullptr;
                return *this;
            }
            if(table[ellement].next!=position)
            {
                position = table[ellement].next;
            }
         }
        
        return *this;
    }
    
    
    Iterator operator++(int) 
    {
       auto iter = *this;
       operator++(); 
       return iter; 
    }
    
    friend bool operator==(const Iterator& lhs, const Iterator& rhs) { return lhs.position == rhs.position; }
    friend bool operator!=(const Iterator& lhs, const Iterator& rhs) { return lhs.position != rhs.position; }
};

template <typename Key, size_t N> void swap(ADS_set<Key,N>& lhs, ADS_set<Key,N>& rhs) { lhs.swap(rhs); }

#endif
