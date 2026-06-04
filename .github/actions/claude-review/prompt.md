Perform a comprehensive code review geared towards expert developers; be very concise and constructive.
If the file exists, also consider the additional project context in `.github/copilot-instructions.md`.
These are the main focus areas for your review:

1. **Code Quality**
   - Clean code principles and best practices
   - DRY (Don't Repeat Yourself) adherence, including refactoring opportunities
     and usage of existing utility functions, libraries, and patterns to reduce boilerplate code
   - Idiomatic usage of all programming languages
   - Proper error and edge case handling
   - Code readability and maintainability
   - Note changes to public APIs that may break downstream consumers

2. **Security**
   - Check for potential security vulnerabilities (injection, XSS, auth, secrets)
   - Review untrusted input parsing and buffer handling for correctness and safety
   - Consider protocol-level attacks (amplification, injection, timing)

3. **Performance**
   - Identify potential performance bottlenecks and optimizations
   - Check for memory leaks or resource and locking issues
   - Check for inefficient algorithms or data structures, suggest alternatives

4. **Testing**
   - Verify sufficient test coverage
   - Review test quality and edge cases
   - Check for missing test scenarios

5. **Documentation**
   - Ensure code is sufficiently - but not overly! - documented; the developers here are experts
   - Check whether comments and documentation are up-to-date and accurate

Read the diff with `gh pr diff` and read changed files for context as needed.

## How to deliver your review (IMPORTANT — this overrides any default posting style)

To avoid anchoring the human reviewer before they form their own opinion, deliver your review
**collapsed**:

- Do NOT post inline comments. Do NOT submit a formal pull-request review. Do NOT approve or
  request changes.
- Post EXACTLY ONE pull-request comment using `gh pr comment <PR> --body-file <file>`.
- The ENTIRE comment body MUST be a single collapsed `<details>` block with this exact structure:

  ```
  <details>
  <summary>🤖 Claude has reviewed this PR — expand after forming your own opinion</summary>

  ...your full review here, organized by the focus areas above, concise, with
  GitHub-suggestion code blocks for proposed fixes...

  </details>
  ```

Rules:
- The `<summary>` line MUST be exactly `🤖 Claude has reviewed this PR — expand after forming your own opinion`
  and MUST NOT reveal any findings, counts, severities, or verdicts.
- NOTHING may appear outside the `<details>` block (no text before `<details>` or after `</details>`).
- There must be a blank line immediately after `</summary>` (required for the body to render).
- If you find no blocking issues, still post the single collapsed comment with a brief note inside.
- Do NOT add any AI attribution, "Co-authored-by", or "Generated with" text.
- Ensure all GitHub-flavored Markdown you emit is syntactically correct.
