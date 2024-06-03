import unittest
import json

from python_src.pipeline import run_pipeline
from python_src.constants import BUILD_DIR, INPUT_FILE

class TestSymbolFilter(unittest.TestCase):
    def test_symbol_filter(self):
        exp_symbol = 'SMDV'
        pipeline = [
            [f'{BUILD_DIR}/symbol_filter', ['-i',INPUT_FILE, '-o', '-', '-s', exp_symbol]],
            [f'{BUILD_DIR}/json', ['-i' '-']],
            ['head', []],
        ]
        exp_res = [-13, -13, 0]
        actual_res, actual_out = run_pipeline(pipeline)
        symbols = set()
        for line in actual_out.strip().split('\n'):
            data = json.loads(line)
            if 'symbol' in data:
                symbols.add(data['symbol'].strip())

        self.assertEqual(symbols, set([exp_symbol]))

        self.assertEqual(exp_res, actual_res)

if __name__ == '__main__':
    unittest.main()
