#ifndef BASE_AUTOPTR_H_
#define BASE_AUTOPTR_H_

#ifndef NAMESPACE_BASE_BEGIN
#define NAMESPACE_BASE_BEGIN namespace base { 
#endif

#ifndef NAMESPACE_END
#define NAMESPACE_END }
#endif


NAMESPACE_BASE_BEGIN

template <typename interface_type>
class com_ptr
{
  void copy(const com_ptr & rhs) { release(); p_ = rhs.p_; if (p_) { p_->AddRef(); } }
  interface_type * p_;
public:
  explicit com_ptr(interface_type * p = NULL) : p_(p) { if (p_) p_->AddRef(); }
  com_ptr(const com_ptr &rhs) { p_ = NULL; copy(rhs); }
  com_ptr & operator = (const com_ptr & rhs) { copy(rhs); return *this; }
  ~com_ptr() { release(); }
  void release() { if (p_) { p_->Release(); p_ = NULL; } }
  bool isempty() const { return (NULL == p_); }
  interface_type * operator -> () { return p_; }
  operator interface_type * () { return p_; }
  interface_type ** get() { return &p_; }
  com_ptr & attach(interface_type * p) { release(); p_ = p; return *this; }
  interface_type * detach() { interface_type * p = p_; p_ = NULL; return p; }
};

template <typename value_type>
class auto_ptr
{
  auto_ptr(const auto_ptr &);
  auto_ptr & operator = (const auto_ptr &);
  value_type * p_;
public:
  explicit auto_ptr(value_type * p = NULL) : p_(p) {}
  ~auto_ptr() { release(); }
  value_type * operator -> ()
  {
    return p_;
  }
  operator value_type * ()
  {
    return p_;
  }
  operator bool() const
  {
    return NULL != p_;
  }
  void release()
  {
    if (p_) delete p_; p_ = NULL;
  }
  void attach(value_type * p)
  {
    release();
    p_ = p;
  }
  value_type * detach()
  {
    value_type * p = p_; p_ = NULL; return p;
  }
  void reset(value_type * p) { release(); p_ = p; }

};



//
// usage
//     struct IA 
//     {
//         virtual void func_a() = 0;
//     };
//     class A : public IA 
//     {
//     public:
//         void func_a() { }
//         void func_self() {}
//     };
// 
//     We got a A's intance 
//     A * p = new A ;
//     We need IA's interface method, also we need A's self method func_self().
//     
//
template<typename ptr_type, typename interface_type>
class part_com_ptr
{
  part_com_ptr(const part_com_ptr &);
  part_com_ptr & operator = (const part_com_ptr &);
protected:
  ptr_type *  ptr_;
  com_ptr<interface_type> com_ptr_;

public:
  part_com_ptr() :ptr_(NULL) {}
  part_com_ptr(ptr_type * p)
  {
    reset(p);
  }
  ~part_com_ptr()
  {
    reset(NULL);
  }

  ptr_type * get_ptr()
  {
    return ptr_;
  }

  com_ptr<interface_type> & get_com_ptr()
  {
    return com_ptr_;
  }

  void reset(ptr_type * p = NULL)
  {
    ptr_ = p;
    com_ptr_ = p;
  }

  bool is_valid() const
  {
    return (ptr_ && com_ptr_._p);
  }

};


NAMESPACE_END; // namespace base

#endif // BASE_AUTOPTR_H_
