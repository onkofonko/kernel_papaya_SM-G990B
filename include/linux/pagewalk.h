/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_PAGEWALK_H
#define _LINUX_PAGEWALK_H

#include <linux/mm.h>

struct mm_walk;

/**
 * mm_walk_ops - callbacks for walk_page_range
 * @pgd_entry:		if set, called for each non-empty PGD (top-level) entry
 * @p4d_entry:		if set, called for each non-empty P4D entry
 * @pud_entry:		if set, called for each non-empty PUD entry
 * @pmd_entry:		if set, called for each non-empty PMD entry
 *			this handler is required to be able to handle
 *			pmd_trans_huge() pmds.  They may simply choose to
 *			split_huge_page() instead of handling it explicitly.
 * @pte_entry:		if set, called for each non-empty PTE (lowest-level)
 *			entry
 * @pte_hole:		if set, called for each hole at all levels
 * @hugetlb_entry:	if set, called for each hugetlb entry
 * @test_walk:		caller specific callback function to determine whether
 *			we walk over the current vma or not. Returning 0 means
 *			"do page table walk over the current vma", returning
 *			a negative value means "abort current page table walk
 *			right now" and returning 1 means "skip the current vma"
 *
 * p?d_entry callbacks are called even if those levels are folded on a
 * particular architecture/configuration.
 */
struct mm_walk_ops {
	int (*pgd_entry)(pgd_t *pgd, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*p4d_entry)(p4d_t *p4d, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pud_entry)(pud_t *pud, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pmd_entry)(pmd_t *pmd, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pte_entry)(pte_t *pte, unsigned long addr,
			 unsigned long next, struct mm_walk *walk);
	int (*pte_hole)(unsigned long addr, unsigned long next,
			struct mm_walk *walk);
	int (*hugetlb_entry)(pte_t *pte, unsigned long hmask,
			     unsigned long addr, unsigned long next,
			     struct mm_walk *walk);
	int (*test_walk)(unsigned long addr, unsigned long next,
			struct mm_walk *walk);
};

/*
 * Action for pud_entry / pmd_entry callbacks.
 * ACTION_SUBTREE is the default
 */
enum page_walk_action {
	/* Descend to next level, splitting huge pages if needed and possible */
	ACTION_SUBTREE = 0,
	/* Continue to next entry at this level (ignoring any subtree) */
	ACTION_CONTINUE = 1,
	/* Call again for this entry */
	ACTION_AGAIN = 2
};

/**
 * mm_walk - walk_page_range data
 * @ops:	operation to call during the walk
 * @mm:		mm_struct representing the target process of page table walk
 * @vma:	vma currently walked (NULL if walking outside vmas)
 * @action:	next action to perform (see enum page_walk_action)
 * @no_vma:	walk ignoring vmas (vma will always be NULL)
 * @private:	private data for callbacks' usage
 *
 * (see the comment on walk_page_range() for more details)
 */
struct mm_walk {
	const struct mm_walk_ops *ops;
	struct mm_struct *mm;
	struct vm_area_struct *vma;
	enum page_walk_action action;
	bool no_vma;
	void *private;
};

int walk_page_range(struct mm_struct *mm, unsigned long start,
		unsigned long end, const struct mm_walk_ops *ops,
		void *private);
int walk_page_range_novma(struct mm_struct *mm, unsigned long start,
			  unsigned long end, const struct mm_walk_ops *ops,
			  void *private);
int walk_page_vma(struct vm_area_struct *vma, const struct mm_walk_ops *ops,
		void *private);

#endif /* _LINUX_PAGEWALK_H */
