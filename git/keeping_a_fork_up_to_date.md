# Keeping a fork up to date


## 1. Clone your fork:

```sh
git clone git@github.com:YOUR-USERNAME/YOUR-FORKED-REPO.git
```

## 2. Add remote from original repository in your forked repository:

```sh
cd into/cloned/fork-repo
git remote add upstream git://github.com/ORIGINAL-DEV-USERNAME/REPO-YOU-FORKED-FROM.git
git fetch upstream
```

## 3. Updating your fork from original repo to keep up with their changes:

```sh
git pull upstream master
```

## 4 git push