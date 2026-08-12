# Contributing

## Building and testing locally

No external dependencies are required by default (the kdb+ C API is
optional — see the README's "Building with a live tickerplant" section).

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Run `./build/feed_handler` to see the demo pipeline end to end. It writes
to `curves_generated.csv` (gitignored) — never to the committed `curves.csv`
seed data, which `q-book-snapshot` reads as a fixture.

## Continuous integration

Every push and pull request runs the `build-and-test` workflow defined in
[`.github/workflows/ci.yml`](.github/workflows/ci.yml) via
[GitHub Actions](https://docs.github.com/actions) — GitHub's built-in
CI/CD system. It runs on a fresh `ubuntu-latest` VM and executes the same
configure/build/test commands as above. Status shows up as a check on the
commit/PR and in the repo's "Actions" tab; the README badge reflects the
latest run on `master`. A PR should have a green check before merging.

## Submitting changes

1. Fork or branch, make your change, and confirm `ctest` passes locally.
2. Open a pull request. You can do this from the GitHub web UI, or from
   the terminal with the [GitHub CLI](https://cli.github.com/) (`gh`):
   ```sh
   gh pr create --title "..." --body "..."
   ```
3. Wait for the `build-and-test` check to pass.

## Style

- Match the existing code: no comments beyond explaining non-obvious
  *why*, small focused functions, no unnecessary abstraction.
- Add a `normalizeSourceN` (see `include/source_normalizer.hpp`) for any
  new raw feed shape rather than branching inside an existing one.
