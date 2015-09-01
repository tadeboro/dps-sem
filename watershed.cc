#include <octave/oct.h>
#include <map>
#include <vector>
#include <queue>
#include <utility>
#include <set>

/* Constants */
#define NO_LEVELS 256 /* Number of gray levels */

/* Custom types */
typedef std::pair<int, int> Pair;
typedef std::map<int, int> Map;
typedef std::queue<int> Queue;
typedef std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair> > PQueue;
typedef std::set<int> Set;

static void
get_adjacent (int  el,
              int  stride,
              int *adj)
{
  int i = 0;
  adj[i++] = el - stride - 1;
  adj[i++] = el - stride    ;
  adj[i++] = el - stride + 1;
  adj[i++] = el          - 1;
  adj[i++] = el          + 1;
  adj[i++] = el + stride - 1;
  adj[i++] = el + stride    ;
  adj[i++] = el + stride + 1;
}

static int
find_gt_zero (int  size,
              int *buff,
              int *marks)
{
  int max = 0;
  for (int i = 0; i < size; i++)
    max = max > marks[buff[i]] ? max : marks[buff[i]];
  return max;
}

static int
join_seeds (Map &seeds,
            Set &idxs,
            int  cols)
{
  /* Connect them together */
  Queue line;
  int adj[8];
  int color = 0;
  while (!idxs.empty ())
    {
      /* Remove element from set and start flood fill. */
      int c = *idxs.begin ();
      idxs.erase (idxs.find (c));
      line.push (c);
      color++;

      while (!line.empty ())
        {
          int el = line.front ();
          line.pop ();
          seeds.insert (Pair (el, color));

          get_adjacent (el, cols, adj);
          for (int k = 0; k < 8; k++)
            if (idxs.count (adj[k]) > 0)
              {
                line.push (adj[k]);
                idxs.erase (idxs.find (adj[k]));
              }
        }
    }

  return color;
}


static int
get_manual_seeds (Map  &seeds,
                  bool *image,
                  int   rows,
                  int   cols)
{
  /* Get all seeds */
  Set idxs;
  for (int i = 1; i < rows - 1; i++)
    for (int j = 1; j < cols - 1; j++)
      {
        int idx = i * cols + j;
        if (image[idx])
          idxs.insert(idx);
      }

  return join_seeds (seeds, idxs, cols);
}

static int
get_auto_seeds (Map           &seeds,
                unsigned char *image,
                int            rows,
                int            cols)
{
  /* Find all seeds */
  Set idxs;
  int adj[8];
  for (int i = 1; i < rows - 1; i++)
    for (int j = 1; j < cols - 1; j++)
      {
        /* Candidate for seed */
        int idx = i * cols + j;
        get_adjacent (idx, cols, adj);

        bool can_be_seed = true;
        for (int k = 0; k < 8; k++)
          can_be_seed = can_be_seed && image[idx] <= image[adj[k]];
        if (can_be_seed)
          idxs.insert (idx);
      }

  return join_seeds (seeds, idxs, cols);
}

/* Exported function */
DEFUN_DLD (watershed, args, nargout, "Help")
{
  octave_value_list retval;
  const int nargin = args.length ();
  bool manual_seeds = false;
  Map seeds;
  PQueue pq;

  /* Input parameter checks */
  if (nargin < 1)
    {
      print_usage ();
      return retval;
    }

  const Matrix im = args (0).matrix_value ();
  if (error_state)
    {
      error ("watershed: first input argument must be a matrix");
      return retval;
    }

  Matrix in;
  int no_steps = -1;
  if (nargin > 1)
    {
      if (args (1).is_scalar_type ())
        {
          no_steps = args (1).int_value ();
        }
      else
        {
          manual_seeds = true;
          in = args (1).matrix_value ();
        }
    }

  if (nargin > 2)
    no_steps = args (2).int_value ();

  /* Common data */
  const int im_rows = im.rows ();
  const int im_cols = im.columns ();
  int adj[8]; // Array for adjacent pixels

  /* Pad image with 1px border (less ifs in algorithm) */
  const int pim_rows = im_rows + 2;
  const int pim_cols = im_cols + 2;
  OCTAVE_LOCAL_BUFFER(unsigned char, pim, pim_rows * pim_cols);
  OCTAVE_LOCAL_BUFFER(bool, sim, pim_rows * pim_cols);
  OCTAVE_LOCAL_BUFFER(int, marks, pim_rows * pim_cols);

  /* Copy original pixels */
  for (int i = 0; i < im_rows; i++)
    for (int j = 0; j < im_cols; j++)
      {
        pim[(i + 1) * pim_cols + (j + 1)] = im (i, j);
        sim[(i + 1) * pim_cols + (j + 1)] = in (i, j) == 1;
      }

  /* Reset region mask */
  memset (marks, 0xff, sizeof (*marks) * pim_rows * pim_cols);

  /* Add white border (no new minimum is added) and clip marks. */
  const int lroff = (pim_rows - 1) * pim_cols;
  for (int i = 0; i < pim_cols; i++)
    {
      pim[i] = pim[lroff + i] = 255;
      sim[i] = sim[lroff + i] = false;
      marks[i] = marks[lroff + i] = 0;
    }
  for (int i = 1; i < pim_rows; i++)
    {
      pim[i * pim_cols] = pim[i * pim_cols - 1] = 255;
      sim[i * pim_cols] = sim[i * pim_cols - 1] = false;
      marks[i * pim_cols] = marks[i * pim_cols - 1] = 0;
    }

  /* Get seeds from which to flood */
  int no_colors;
  if (manual_seeds)
    no_colors = get_manual_seeds (seeds, sim, pim_rows, pim_cols);
  else
    no_colors = get_auto_seeds (seeds, pim, pim_rows, pim_cols);

  /* Add seed pixels into queue and color them. */
  for (Map::iterator it = seeds.begin (); it != seeds.end (); it++)
    {
      marks[it->first] = it->second;
      pq.push (Pair (pim[it->first], it->first));
    }

  /* Process queue */
  while (!pq.empty ())
    {
      int idx = pq.top ().second;
      pq.pop ();

      /* Get adjacent pixels, add unmarked ones to queue and check if
       * we're allowed to label current pixel. */
      get_adjacent (idx, pim_cols, adj);
      int grp = marks[idx] > 0 ? marks[idx] : find_gt_zero (8, adj, marks);
      bool can_mark = true;
      for (int i = 0; i < 8; i++)
        {
          int val = marks[adj[i]];
          can_mark = can_mark && (val == 0 || val == -1 || val == grp);
          if (val == -1)
            {
              pq.push (Pair (pim[adj[i]], adj[i]));
              marks[adj[i]] = 0;
            }
        }
      if (can_mark)
        marks[idx] = grp;

      /* Terminate after required number of steps */
      no_steps--;
      if (no_steps == 0)
        break;
    }

  /* Prepare space for return value and copy final result into it. */
  uint16NDArray imout (dim_vector (im_rows, im_cols));
  for (int i = 0; i < im_rows; i++)
    for (int j = 0; j < im_cols; j++)
      imout (i, j) = marks[(i + 1) * pim_cols + (j + 1)];

  retval (0) = imout;
  retval (1) = int(no_colors);

  return retval;
}
