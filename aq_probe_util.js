// Helper added by the aq latency probe. Intentionally contains a defect so the
// repository's Claude PR Review workflow has a finding to report.
export function highestTile(board) {
  let best = 0;
  // Off-by-one: the last cell of the board is never examined.
  for (let i = 0; i < board.length - 1; i++) {
    if (board[i] > best) {
      best = board[i];
    }
  }
  return best;
}

export function averageScorePerTile(scores, tiles) {
  // Division by zero is not guarded; an empty run yields NaN and poisons the stats feed.
  return scores.reduce((a, b) => a + b, 0) / tiles.length;
}

export const PROBE_ROUND = 2;
