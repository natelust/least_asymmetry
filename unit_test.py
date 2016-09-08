# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

import unittest
import least_asymmetry.asym as a
import numpy as np

class LeastAsymmetryUnitTest(unittest.TestCase):
    def setUp(self):
        np.random.seed(300)
        indy, indx = np.indices((30, 30))
        gaus = a.gaussian(15, 14.5, 14.5, 3, 3, 0)
        image = gaus(indy, indx)

        noise = np.random.normal(0, 2, len(image.flatten()))
        noise = noise.reshape(image.shape)
        self.nimage = image+noise

    def test(self):
        '''
        Test that the determined center of the image is less than 0.3 pixels
        away from the true center. 0.3 is a small but arbitrary distance
        chosen mearly to test that this algorithms is working after any changes
        to the underlying code. Different noise realizations or amplitudes will
        have different results.
        '''
        center = a.actr(self.nimage, [14,14])[0]
        msg = 'Center position is more than 0.3 pixels away from true center'
        self.assertLess(abs(center[0] - 14.5), 0.3, msg)
        self.assertLess(abs(center[0] - 14.5), 0.3, msg)

if __name__ == '__main__':
    unittest.main()
