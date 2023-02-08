extern "C" {

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <hb.h>
#include <hb-ot.h>
#include <sys/resource.h>

static PyObject *
font_profile(PyObject *self, PyObject *args)
{
    const char *fontfile;
    const char *text;
    int sts;
    struct rusage usage;
    long rss_before;
    long rss_after_blob;
    long rss_after_face;
    long rss_after_font;
    long rss_after_shape;

    getrusage(RUSAGE_SELF, &usage);
    rss_before = usage.ru_maxrss;

    if (!PyArg_ParseTuple(args, "ss", &fontfile, &text))
        return NULL;

    hb_blob_t* blob = hb_blob_create_from_file_or_fail(fontfile);
    if (blob == NULL) {
         PyErr_SetString(PyExc_ValueError, "Couldn't create font blob");
         return (PyObject *) NULL;
    }

    getrusage(RUSAGE_SELF, &usage);
    rss_after_blob = usage.ru_maxrss;

    hb_face_t* face = hb_face_create(blob, 0);
    if (face == NULL) {
         PyErr_SetString(PyExc_ValueError, "Couldn't create font face");
         return (PyObject *) NULL;
    }
    getrusage(RUSAGE_SELF, &usage);
    rss_after_face = usage.ru_maxrss;

    hb_font_t* font = hb_font_create(face);
    hb_ot_font_set_funcs(font);
    getrusage(RUSAGE_SELF, &usage);
    rss_after_font= usage.ru_maxrss;

    hb_buffer_t* buf = hb_buffer_create();
    hb_buffer_add_utf8(buf, text, strlen(text), 0, -1);
    hb_buffer_guess_segment_properties(buf);
    hb_shape(font, buf, NULL, 0);
    getrusage(RUSAGE_SELF, &usage);
    rss_after_shape = usage.ru_maxrss;

    hb_buffer_destroy(buf);
    hb_font_destroy(font);
    hb_face_destroy(face);
    hb_blob_destroy(blob);
    return Py_BuildValue("{s:l,s:l,s:l,s:l,s:l}",
        "before", rss_before,
        "after_blob", rss_after_blob - rss_before,
        "after_face", rss_after_face - rss_after_blob,
        "after_font", rss_after_font - rss_after_face,
        "after_shape", rss_after_shape - rss_after_font
    );

}

static PyMethodDef FontProfileMethods[] = {
    {"profile",  font_profile, METH_VARARGS,
     "Shape text with a font and return memory usage"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef fontprofilemodule = {
    PyModuleDef_HEAD_INIT,
    "fontprofile",
    NULL,
    -1,
    FontProfileMethods
};

PyMODINIT_FUNC
PyInit_fontprofile(void)
{
    return PyModule_Create(&fontprofilemodule);
}

}
