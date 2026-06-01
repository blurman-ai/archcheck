#!/usr/bin/env python3
# sample_lowstar.py — стратифицированная выборка НИЗКО-звёздных C++ AIDev-реп.
# Гипотеза (#054): меньше звёзд -> меньше глаз на ревью -> хуже арх-дисциплина.
# Чтобы увидеть градиент, берём по бакетам звёзд, а не один низкий band.
# Детерминированно (seed=42). Размерный фильтр <500MB — на этапе клонирования
# (в parquet нет колонки size), здесь только пул кандидатов по бакетам.
import pandas as pd

PARQUET = '/tmp/aidev_repo.parquet'
OUT = '~/projects/cpparch/experiments/ai_repo_run/lowstar_pool.tsv'
BUCKETS = [(0, 2), (2, 10), (10, 30), (30, 100)]  # [100,inf) = уже есть (corpus_50)

df = pd.read_parquet(PARQUET)
cpp = df[df['language'] == 'C++'].copy()
cpp['stars'] = pd.to_numeric(cpp['stars'], errors='coerce').fillna(0).astype(int)

rows = []
for lo, hi in BUCKETS:
    b = cpp[(cpp['stars'] >= lo) & (cpp['stars'] < hi)].copy()
    b = b.sample(frac=1.0, random_state=42).reset_index(drop=True)  # детерминир. шафл
    tag = f'{lo}-{hi}'
    for _, r in b.iterrows():
        rows.append((tag, r['full_name'], int(r['stars'])))
    print(f'bucket {tag:>7}: {len(b)} candidates')

with open(OUT, 'w') as f:
    f.write('bucket\tfull_name\tstars\n')
    for tag, name, stars in rows:
        f.write(f'{tag}\t{name}\t{stars}\n')
print('written', OUT, '(', len(rows), 'candidates total )')
