import unittest
import tempfile
import os

from python_src.pipeline import run_pipeline
from python_src.constants import BUILD_DIR, INPUT_FILE

class TestListSymbols(unittest.TestCase):
    def test_stream_split(self):
        with tempfile.TemporaryDirectory() as tmpdir:
            pipeline = [[f'{BUILD_DIR}/stream_split', ['-i', INPUT_FILE, '-o', tmpdir]]]
            exp_res = [0]
            actual_res, _ = run_pipeline(pipeline)

            self.assertEqual(exp_res, actual_res)
            self.assertEqual(len(os.listdir(tmpdir)), 6)



if __name__ == '__main__':
    unittest.main()
