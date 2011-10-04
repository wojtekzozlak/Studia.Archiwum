#include "page.h"
#include "strategy.h"

page* select_page(page *page)
{
  struct page *min_hits = page;
  while(page != NULL)
  {
    if(page->hits < min_hits->hits)
      min_hits = page;
    page = page->next;
  }
  return min_hits;
}
