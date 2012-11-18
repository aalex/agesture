#include "nearestneighborfinder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <iostream>
#include <glib.h>

// constants:
static const guint NUM_SPECTRAL_BANDS = 20;
static const unsigned int LENGTH = 3;
static const unsigned int DIMENSIONS = NUM_SPECTRAL_BANDS;
static const char * const SPECTRUM_ELEMENT_NAME = "spectrum";
static const char * const AUDIO_SOURCE_TYPE = "jackaudiosrc";
static const int AUDIO_SAMPLING_RATE = 48000;
static bool VERBOSE = false;

// classes:
class AudioGestureApp
{
    public:
        AudioGestureApp() :
            finder_(DIMENSIONS, LENGTH)
        {
            std::cout << __FUNCTION__ << std::endl;
        }
        void handleSpectrum(const std::vector<double> &amplitudes);
        bool startLearning(const char *name);
        bool startDetecting();
    private:
        agesture::NearestNeighborFinder finder_;
        bool is_detecting_;
        bool is_learning_;
        agesture::NearestNeighborFinder::Path current_learn_or_detect_path_;
        std::string name_of_current_record_path_;
};

bool AudioGestureApp::startLearning(const char *name)
{
    if (this->is_learning_ || this->is_detecting_)
    {
        if (this->is_detecting_)
            std::cout << "is already detecting" << std::endl;
        else
            std::cout << "is already learning" << std::endl;
        return false;
    }
    this->is_learning_ = true;
    this->name_of_current_record_path_ = std::string(name);
    return true;
}

bool AudioGestureApp::startDetecting()
{
    if (this->is_learning_ || this->is_detecting_)
    {
        if (this->is_detecting_)
            std::cout << "is already detecting" << std::endl;
        else
            std::cout << "is already learning" << std::endl;
        return false;
    }
    this->is_detecting_ = true;
    return true;
}

void AudioGestureApp::handleSpectrum(const std::vector<double> &amplitudes)
{
    if (this->is_learning_ || this->is_detecting_)
    {
        // store it
        if (this->current_learn_or_detect_path_.size() < LENGTH)
            this->current_learn_or_detect_path_.push_back(amplitudes);
        else
        {
            // learn it
            if (this->is_learning_)
            {
                bool ok = this->finder_.learn(this->name_of_current_record_path_.c_str(), this->current_learn_or_detect_path_);
                if (! ok)
                {
                    std::cout << "error learning!" << std::endl;
                }
            }
            else // try to find a learned that matches
            {
                std::string name;
                if (this->finder_.findClosest(this->current_learn_or_detect_path_, name))
                {
                    std::cout << "found match " << name << std::endl;
                }
                else
                {
                    std::cout << "did not find match" << std::endl;
                }
            }
            this->current_learn_or_detect_path_.clear();
        }
    }
}

// functions:

/* receive spectral data from element message */
static gboolean message_handler(GstBus * bus, GstMessage * message, gpointer data)
{
    (void) bus;
    if (message->type == GST_MESSAGE_ELEMENT)
    {
        const GstStructure *s = gst_message_get_structure(message);
        const gchar *name = gst_structure_get_name(s);
        GstClockTime endtime;

        if (g_strcmp0(name, SPECTRUM_ELEMENT_NAME) == 0)
        {
            //gdouble freq;
            guint i;

            if (! gst_structure_get_clock_time(s, "endtime", &endtime))
            {
                endtime = GST_CLOCK_TIME_NONE;
            }

            //g_print("New spectrum message, endtime %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(endtime));

            const GValue *magnitudes = gst_structure_get_value(s, "magnitude");
            const GValue *phases = gst_structure_get_value(s, "phase");

            if (VERBOSE)
                g_print("spectrum: ");
            std::vector<double> vec;
            for (i = 0; i < NUM_SPECTRAL_BANDS; ++i)
            {
                //freq = (gdouble) ((AUDIO_SAMPLING_RATE / 2) * i + AUDIO_SAMPLING_RATE / 4) / NUM_SPECTRAL_BANDS;
                const GValue *mag = gst_value_list_get_value(magnitudes, i);
                const GValue *phase = gst_value_list_get_value(phases, i);
                if (mag != NULL && phase != NULL)
                {
                    //g_print("band %d (freq %g): magnitude %f dB phase %f\n", i, freq, g_value_get_float(mag), g_value_get_float(phase));
                    vec.push_back((double) g_value_get_float(mag));
                    if (VERBOSE)
                        g_print("%f ", g_value_get_float(mag));
                }
            }
            if (VERBOSE)
                g_print("\n");
            AudioGestureApp *app = (AudioGestureApp *) data;
            app->handleSpectrum(vec);
        }
    }
    return TRUE;
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    AudioGestureApp app;

    GstElement *bin;
    GstElement *src, *audioconvert, *spectrum, *sink;
    GstBus *bus;
    GMainLoop *loop;

    gst_init(&argc, &argv);

    bin = gst_pipeline_new("bin");
    src = gst_element_factory_make(AUDIO_SOURCE_TYPE, "src");
    audioconvert = gst_element_factory_make("audioconvert", NULL);
    g_assert(audioconvert);

    spectrum = gst_element_factory_make("spectrum", SPECTRUM_ELEMENT_NAME);
    g_assert(spectrum); 
    g_object_set(G_OBJECT(spectrum),
        "bands", NUM_SPECTRAL_BANDS,
        "threshold", -80,
        //"interval", 10 * 1000000, // 10 ms
        "message", TRUE,
        "message-phase", TRUE,
        NULL);
    sink = gst_element_factory_make("fakesink", "sink");
    g_assert(sink); 
    g_object_set(G_OBJECT(sink), "sync", TRUE, NULL);

    gst_bin_add_many(GST_BIN(bin), src, audioconvert, spectrum, sink, NULL);


    if (! gst_element_link(src, audioconvert))
    {
        fprintf(stderr, "can't link src to audioconvert\n");
        exit(1);
    }
    // TODO: use the caps
    GstCaps *caps = gst_caps_new_simple("audio/x-raw", "rate", G_TYPE_INT, AUDIO_SAMPLING_RATE, NULL);
    if (! gst_element_link(audioconvert, spectrum))
    //if (! gst_element_link_filtered(audioconvert, spectrum, caps))
    {
        fprintf(stderr, "can't link audioconvert to spectrum\n");
        exit(1);
    }
    gst_caps_unref(caps);
    if (! gst_element_link(spectrum, sink))
    {
        fprintf(stderr, "can't link spectrum to sink\n");
        exit(1);
    }

    bus = gst_element_get_bus(bin);
    gst_bus_add_watch(bus, message_handler, (gpointer) &app);
    gst_object_unref(bus);

    gst_element_set_state(bin, GST_STATE_PLAYING);

    /* we need to run a GLib main loop to get the messages */
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    gst_element_set_state(bin, GST_STATE_NULL);
    gst_object_unref(bin);

    return 0;
}

