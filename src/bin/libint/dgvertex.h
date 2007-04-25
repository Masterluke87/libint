
#ifndef _libint2_src_bin_libint_dgvertex_h_
#define _libint2_src_bin_libint_dgvertex_h_

#include <dg.h>
#include <drtree.h>
#include <dgarc.h>
#include <iostream>
#include <string>
#include <smart_ptr.h>
#include <memory.h>
#include <exception.h>
#include <class_registry.h>
#include <singl_stack.h>
#include <hashable.h>
#include <libint2_intrinsic_types.h>
#include <key.h>

namespace libint2 {

  /// This is a vertex of a Directed Graph (DG)
  class DGVertex : public Hashable<LIBINT2_UINT_LEAST64,ComputeKey> {
  public:
    /// The address on the stack during computation is described using this type
    typedef MemoryManager::Address Address;
    /// The size of a block the stack during computation is described using this type
    typedef MemoryManager::Size Size;
    /// Exception thrown if address is not set
    typedef NotSet<Address> AddressNotSet;
    /// Exception thrown if graph label is not set
    typedef NotSet<std::string> GraphLabelNotSet;
    /// Exception thrown if code symbol is not set
    typedef NotSet<std::string> SymbolNotSet;
    /// Type identifier
    typedef ClassRegistry::ClassID ClassID;
    /// Instance identifier
    typedef GSingletonTrait::InstanceID InstanceID;
    /** DGVertex provides function key() which computes key of type KeyType and
        returns it using KeyReturnType */
    typedef LIBINT2_UINT_LEAST64 KeyType;
    typedef Hashable<KeyType,ComputeKey>::KeyReturnType KeyReturnType;

    /** typeid stores the ClassID of the concrete type. It is used to check quickly whether
        2 DGVertices are of the same type. Dynamic casts are too expensive. */
    ClassID typeid_;
    /** instid stores the InstanceID of the object. Only makes sense for Singletons.
        For other objects it's zero. Can be used to compare objects quickly. */
    InstanceID instid_;
    /// Sets typeid to tid
    DGVertex(ClassID tid);
    /// Sets typeid to tid 
    DGVertex(ClassID tid, const vector<SafePtr<DGArc> >& parents, const vector<SafePtr<DGArc> >& children);
    /// This is a copy constructor
    DGVertex(const DGVertex& v);
    virtual ~DGVertex();

    /// make_a_target() marks this vertex as a target
    void make_a_target();
    /// is_a_target() returns true if this vertex is a target
    const bool is_a_target() const { return target_;};
    /** add_exit_arc(arc) adds arc as an arc connecting to children of this vertex.
        Thus, arcs are owned by their PARENTS. This function is virtual because
        certain types of vertices have duplicate references to children in their
        definition (such as AlgebraicOperator). Such DGVertices need to update their
        private members.
      */
    virtual void add_exit_arc(const SafePtr<DGArc>&);
    /** del_exit_arcs() removes all exit arcs from this and corresponding children vertices.
        See documentation for del_exit_arc().
      */
    virtual void del_exit_arcs();
    /** replace_exit_arc() replaces A with B. See documentation for del_exit_arc().
     */
    void replace_exit_arc(const SafePtr<DGArc>& A, const SafePtr<DGArc>& B);
    /** this function detaches the vertex from other vertices. It cannot safely remove
        entry arcs, so the user must previously delete or replace them (see documentation for del_exit_arc()).
	can throw CannotPerformOperation.
    */
    void detach();

    /// returns the number of parents
    unsigned int num_entry_arcs() const;
    /// returns ptr to i-th parent
    SafePtr<DGArc> entry_arc(unsigned int) const;
    /// returns the number of children
    unsigned int num_exit_arcs() const;
    /// returns ptr to i-th child
    SafePtr<DGArc> exit_arc(unsigned int) const;
    /// return arc connecting this to v, otherwise null pointer
    SafePtr<DGArc> exit_arc(const SafePtr<DGVertex>& v) const;

    /// computes key
    virtual KeyReturnType key() const =0;
    /** equiv(const DGVertex* aVertex) returns true if this vertex is
        equivalent to *aVertex.
    */
    virtual bool equiv(const SafePtr<DGVertex>&) const =0;
    
    /** precomputed() returns whether this DGVertex is precomputed. See
    precomputed_this() for description.
    */
    bool precomputed() const;

    /** Returns the amount of memory (in floating-point words) to be allocated for the vertex.
      */
    virtual const unsigned int size() const =0;
    
    /** label() returns a unique, short, descriptive label of DGVertex (e.g. "( p_x s | 1/r_{12} | d_xy s )^{(1)}")
    */
    virtual const std::string& label() const =0;
    /** is() returns a very short label of DGVertex which is (almost)
        guaranteed to be a symbol (e.g. "(p_x s|d_xy s)^1")
    */
    virtual const std::string& id() const =0;
    /** description() returns a full, human-readable description of DGVertex (e.g. "This is a ( p_x s | 1/r_{12} | d_xy s )^{(1)} integral")
    */
    virtual const std::string& description() const =0;
    /** print(os) prints vertex info to os */
    void print(std::ostream& os) const;

    /// Returns pointer to the DirectedGraph to which this DGVertex belongs to
    const DirectedGraph* dg() const { return dg_; }
    /// Sets pointer to the DirectedGraph to which this DGVertex belongs to. Should be used with utmost caution
    void dg(const DirectedGraph* d) { dg_ = d; }
    /// returns the label used for this vertex when visualizing graph. can throw GraphLabelNotSet.
    const std::string& graph_label() const;
    /// sets the graph label
    void set_graph_label(const std::string& graph_label);
    
    /// Returns the subtree to which this vertex belongs
    const SafePtr<DRTree>& subtree() const { return subtree_; }

    //
    // NOTE : the following functions probably belong to a separate class, such as Entity!
    //
    
    /**
    refer_this_to(V) makes this vertex act like a reference to V so that
    calls to symbol() and address() report code symbol and stack address of V
    */
    void refer_this_to(const SafePtr<DGVertex>&  V);
    /// returns the code symbol. can throw SymbolNotSet
    const std::string& symbol() const;
    /// sets the code symbol
    void set_symbol(const std::string& symbol);
    /// returns true if the symbol has been set
    bool symbol_set() const { return !symbol_.empty(); }
    /// this function void the symbol, i.e. it is no longer set after calling this member
    void reset_symbol();
    /// returns the address of this quantity on Libint's stack. can throw AddressNotSet
    Address address() const;
    /// sets the address of this quantity on Libint's stack
    void set_address(const Address& address);
    /// returns true if the address has been set
    bool address_set() const { return address_ >= 0; }
    /** indicates whether this vertex needs to be computed.
        Even if this vertex is not precomputed, it may not be desired
        to compute it. By default, all vertices need to be computed.
      */
    void need_to_compute(bool ntc);
    /// shortcut to need_to_compute(false)
    void not_need_to_compute() { need_to_compute(false); }
    /// returns true if this index needs to be computed.
    bool need_to_compute() const;
    
    /// prepare_to_traverse() must be called before traversal of the graph starts
    void prepare_to_traverse();
    /// tag() tags the vertex and returns the total number of tags this vertex has received
    unsigned int tag();
    /// Returns pointer to vertex to be computed after this vertex, 0 if this is the last vertex
    SafePtr<DGVertex> postcalc() const { return postcalc_; };
    /// Sets postcalc
    void set_postcalc(const SafePtr<DGVertex>& postcalc) { postcalc_ = postcalc; };

    /// Resets the vertex, releasing all arcs
    void reset();
    /// If vertex is a singleton then remove it from the SingletonManager. Must be reimplemented in derived singleton class
    virtual void unregister() const;

  protected:
    /// ArcSetType is a container used to maintain entry and exit arcs
    typedef vector< SafePtr<DGArc> > ArcSetType;

    /** this_precomputed() is used by precomputed() to determine whether this
        object really is precomputed. E.g. (ss|ss) shell is considered not
        precomputed, i.e. precomputed_this() will return false. But the (ss|ss)
        integral is considered precomputed. Usually the shell vertex
        will refer to the integral vertex. Thus calling precomputed() on it
        will return true. 
    */
    virtual bool this_precomputed() const =0;

  private:
    /// the pointer to the graph to which this vertex belongs (can be null)
    const DirectedGraph* dg_;
    /// label for the vertex within the graph
    std::string graph_label_;

    /// if not null -- use this vertex to report address and symbol
    const DGVertex* referred_vertex_;
    /// number of vertices which refer to this
    unsigned int nrefs_;
    /// increments number of references
    void inc_nrefs();
    
    /// symbol used in the code
    std::string symbol_;
    /// Address on the stack
    Address address_;
    // Whether this vertex needs to be computed
    bool need_to_compute_;
    
    /// We also need info about Arcs entering this DGVertex
    ArcSetType parents_;
    /// Arcs leaving this DGVertex. Derived classes may need direct access to exit arcs.
    ArcSetType children_;

    // Whether this is a "target" vertex, i.e. the target of a calculation
    bool target_;
    // If set to true -- traversal has started and add_entry... cannot be called
    bool can_add_arcs_;

    /** del_exit_arc(arc) removes arc c (from this and corresponding child).
        NOTE: This function is private because for some classes order of exit arcs
        matters (such as noncommutative AlgebraicOperator). Thus it may not be safe
        to use del_exit_arc() to replace an arc. replace_exit_arc() should be used
        in such instances.
      */
    void del_exit_arc(const SafePtr<DGArc>& c);
    /// add_entry_arc(arc) adds arc as an arc connecting parents to this vertex
    void add_entry_arc(const SafePtr<DGArc>&);
    /// del_entry_arc(arc) removes arc as an arc connecting parents to this vertex
    void del_entry_arc(const SafePtr<DGArc>&);

    ////////
    // These members used in traversal algorithms
    ////////

    /// num_tagged_arcs keeps track of how many entry arcs have been tagged during traversal
    unsigned int num_tagged_arcs_;
    /// Which DGVertex to be computed after this vertex (0, if this is the last vertex)
    SafePtr<DGVertex> postcalc_;
    
    
    ///////
    // Refback to subtree which contains this vertex
    ///////
    
    // note that this is a refback (back reference to the "owning" object) so changing it
    // does not change class invariant -- hence mutable
    
    /// the subtree which contains this vertex (may be null). subtree is a directed rooted tree.
    mutable SafePtr<DRTree> subtree_;
    /// Only DRTree::set_subtree and DRTree::detach_from can change subtree_
    friend void DRTree::add_vertex(const SafePtr<DGVertex>& vertex);
    friend void DRTree::detach_from(const SafePtr<DGVertex>& v);
    
  };

  //
  // Nonmember predicates
  //
  /// return true if V is an unrolled IntegralSet
  struct UnrolledIntegralSet : public std::unary_function<const SafePtr<DGVertex>&,bool> {
    bool operator()(const SafePtr<DGVertex>& V);
  };
  /// return false if V is an unrolled IntegralSet
  struct NotUnrolledIntegralSet : public std::unary_function<const SafePtr<DGVertex>&,bool> {
    bool operator()(const SafePtr<DGVertex>& V);
  };
  /// return true if V is an Integral in an unrolled target IntegralSet
  struct IntegralInTargetIntegralSet : public std::unary_function<const SafePtr<DGVertex>&,bool> {
    bool operator()(const SafePtr<DGVertex>& V);
  };

  /// this composite hashing key works for DGVertex
  typedef TypeAndInstance<DGVertex::ClassID,DGVertex::InstanceID> DGVertexKey;
  inline DGVertexKey key(const DGVertex& v) {
    return DGVertexKey(v.typeid_,v.instid_);
  }
  
};

#endif

