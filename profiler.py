import sys
from fontprofile import profile
import os
import gc
import humanize

args = sys.argv[1:]


def pairwise(t):
    it = iter(t)
    return zip(it, it)


# Prime the pump
profile(__file__, "abc")

for font, text in pairwise(args):
    print(os.path.basename(font), text)
    result = profile(font, text)
    print(result)
    total = (
        result["after_blob"]
        + result["after_face"]
        + result["after_font"]
        + result["after_shape"]
    )
    print("Total physical memory increase:", humanize.naturalsize(total))
