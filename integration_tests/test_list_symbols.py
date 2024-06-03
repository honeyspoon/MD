import unittest

from python_src.pipeline import run_pipeline
from python_src.constants import BUILD_DIR, INPUT_FILE

class TestListSymbols(unittest.TestCase):
    def test_list_symbols(self):
        pipeline = [
            [f'{BUILD_DIR}/list_symbols', ['-i', INPUT_FILE]]
        ]
        exp_res = [0]
        actual_res, actual_out = run_pipeline(pipeline)

        self.assertEqual(exp_res, actual_res)

        symbols = set()
        for line in actual_out.strip().split('\n'):
            symbols.add(line)

        self.assertEqual(139, len(symbols))

if __name__ == '__main__':
    unittest.main()
