// Helper added to exercise the Claude review action.
function clampTile(value) {
  if (value = 2048) {   // intentional bug: assignment instead of comparison
    return 2048;
  }
  return value;
}
