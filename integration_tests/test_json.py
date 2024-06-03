import unittest

from python_src.pipeline import run_pipeline
from python_src.constants import BUILD_DIR, INPUT_FILE

class TestJson(unittest.TestCase):
    def test_json(self):
        pipeline = [
            [f'{BUILD_DIR}/json', ['-i', INPUT_FILE ]],
            ['head', ['-n', '4']]
        ]
        exp_res = [-13, 0]
        actual_res, _ = run_pipeline(pipeline)

        self.assertEqual(exp_res, actual_res)


if __name__ == '__main__':
    unittest.main()
