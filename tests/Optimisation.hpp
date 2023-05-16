#ifndef __OPTIMISATION_HPP__
#define __OPTIMISATION_HPP__
#endif

/*
 * Optimize directives for minmax_normalise()
 * _MINMAX_NORMALIZE_OPT_ : 0 -> no optimization
 * _MINMAX_NORMALIZE_OPT_ : 1 -> Loop unrolling
 * _MINMAX_NORMALIZE_OPT_ : 2 -> Loop fusion
 * _MINMAX_NORMALIZE_OPT_ : 3 -> Loop interchange
 */
#ifndef _MINMAX_NORMALIZE_OPT_
#define _MINMAX_NORMALIZE_OPT_ 3
#endif

/*
 * Optimize directives for minmax()
 * _MINMAX_OPT_ : 0 -> no optimization
 * _MINMAX_OPT_ : 1 -> Loop unrolling and array blocking
 * _MINMAX_OPT_ : 2 -> Loop fusion
 * _MINMAX_OPT_ : 3 -> Loop tiling
 * _MINMAX_OPT_ : 4 -> Loop interchange
 */
#ifndef _MINMAX_OPT_
#define _MINMAX_OPT_ 1
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

/*
 * Optimize directives for NodeData::update()
 * _UPDATE_OPT_ : 0 -> no optimization
 * _UPDATE_OPT_ : 1 -> Loop unrolling
 */
#ifndef _UPDATE_OPT_
#define _UPDATE_OPT_ 1
#endif

/*
 * Optimize directives for NodeData::std::tuple<bool, attribute_index_t, data_t,
 * data_t> evaluateSplit()
 * _EVALUALTE_SPLIT_OPT_ : 0 -> no optimization
 * _EVALUALTE_SPLIT_OPT_ : 1 -> Loop unrolling
 */
#ifndef _EVALUALTE_SPLIT_OPT_
#define _EVALUALTE_SPLIT_OPT_ 1
#endif

/*
 * Optimize directives for void _updateQuantiles(attribute_index_t
 * attributeIndex, class_index_t classif, data_t value) function.
 * _UPDATE_QUANTILES_OPT_ : 0 -> no optimization
 * _UPDATE_QUANTILES_OPT_ : 1 -> local variable to reduce memory access
 * _UPDATE_QUANTILES_OPT_ : 2 -> local variable and loop unrolling
 */
#ifndef _UPDATE_QUANTILES_OPT_
#define _UPDATE_QUANTILES_OPT_ 2
#endif