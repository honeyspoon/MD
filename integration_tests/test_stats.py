import unittest
import json

from python_src.pipeline import run_pipeline
from python_src.constants import BUILD_DIR, INPUT_FILE

class TestStats(unittest.TestCase):
    def test_stats(self):
        pipeline = [[f'{BUILD_DIR}/stats', ['-i',INPUT_FILE, '-j']]]
        exp_res = [0]
        actual_res, actual_out = run_pipeline(pipeline)

        total = 0
        for line in actual_out.strip().split('\n'):
            stat = json.loads(line.strip())
            total += stat['accepted']

        self.assertEqual(exp_res, actual_res)
        self.assertEqual(total, 50069)

if __name__ == '__main__':
    unittest.main()
