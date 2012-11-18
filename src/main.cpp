#include "nearestneighborfinder.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <glib.h>
#include <clutter/clutter.h>

// constants:
static const guint NUM_SPECTRAL_BANDS = 20;
static const unsigned int LENGTH = 3;
static const unsigned int DIMENSIONS = NUM_SPECTRAL_BANDS;
static const char * const SPECTRUM_ELEMENT_NAME = "spectrum";
static const char * const AUDIO_SOURCE_TYPE = "jackaudiosrc";
static const char * const FREQ_RECTANGLE_PREFIX = "freq_";
static const int AUDIO_SAMPLING_RATE = 48000;
static bool VERBOSE = false;
static const ClutterColor BLACK = { 0x00, 0x00, 0x00, 0xff };
static const ClutterColor RED = { 0xcc, 0x33, 0x33, 0xff };
static const ClutterColor GREEN = { 0x33, 0xcc, 0x33, 0xff };
static const ClutterColor GRAY = { 0x99, 0x99, 0x99, 0xff };
static const ClutterColor WHITE = { 0xff, 0xff, 0xff, 0xff };
static const ClutterColor LIGHT_GRAY = { 0xcc, 0xcc, 0xcc, 0xff };

// function definitions
static void key_event_cb(ClutterActor *actor, ClutterKeyEvent *event, gpointer user_data);

// Clutter legacy macro aliases:
#if CLUTTER_CHECK_VERSION(1,4,0)
#else
#define CLUTTER_KEY_Down CLUTTER_Down
#define CLUTTER_KEY_Escape CLUTTER_Escape
#define CLUTTER_KEY_F1 CLUTTER_F1
#define CLUTTER_KEY_Left CLUTTER_Left
#define CLUTTER_KEY_Return CLUTTER_Return
#define CLUTTER_KEY_Right CLUTTER_Right
#define CLUTTER_KEY_Tab CLUTTER_Tab
#define CLUTTER_KEY_Up CLUTTER_Up
#define CLUTTER_KEY_q CLUTTER_q
#define CLUTTER_KEY_space CLUTTER_space
#define CLUTTER_KEY_Delete CLUTTER_Delete
#define CLUTTER_KEY_BackSpace CLUTTER_BackSpace
#define CLUTTER_KEY_0 CLUTTER_0
#define CLUTTER_KEY_1 CLUTTER_1
#define CLUTTER_KEY_2 CLUTTER_2
#define CLUTTER_KEY_3 CLUTTER_3
#define CLUTTER_KEY_4 CLUTTER_4
#define CLUTTER_KEY_5 CLUTTER_5
#define CLUTTER_KEY_6 CLUTTER_6
#define CLUTTER_KEY_7 CLUTTER_7
#define CLUTTER_KEY_8 CLUTTER_8
#define CLUTTER_KEY_9 CLUTTER_9
#endif

// classes:
class AudioGestureApp
{
    public:
        AudioGestureApp() :
            finder_(DIMENSIONS, LENGTH)
        {
            std::cout << __FUNCTION__ << std::endl;
        }
        bool launch()
        {
            return createGUI();
        }
        void handleSpectrum(const std::vector<double> &amplitudes);
        bool startLearning(const char *name);
        bool startDetecting();
    private:
        agesture::NearestNeighborFinder finder_;
        bool is_detecting_;
        bool is_learning_;
        agesture::NearestNeighborFinder::Path current_learn_or_detect_path_;
        std::string name_of_current_learn_path_;
        ClutterActor *detect_button_;
        ClutterActor *learn_button_;
        ClutterActor *stage_;
        ClutterActor *detected_number_text_;
        bool createGUI();
};

static void on_frame_cb(ClutterTimeline * /*timeline*/, guint * /*ms*/, gpointer user_data)
{
    //App *context = static_cast<App*>(user_data);
    // context->poll();
}

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
    this->name_of_current_learn_path_ = std::string(name);
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
    for (unsigned int i = 0; i < NUM_SPECTRAL_BANDS; i++)
    {
        std::ostringstream os;
        os << FREQ_RECTANGLE_PREFIX;
        os << i;
        std::cout << os.str();
        ClutterActor *rect = clutter_container_find_child_by_name(CLUTTER_CONTAINER(this->stage_), os.str().c_str());
        double amp = amplitudes[i];
        clutter_actor_set_height(rect, std::abs(amp * 100000));
    }


    // TODO:
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
                bool ok = this->finder_.learn(this->name_of_current_learn_path_.c_str(), this->current_learn_or_detect_path_);
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

static void key_event_cb(ClutterActor *actor, ClutterKeyEvent *event, gpointer user_data)
{
    AudioGestureApp *app = (AudioGestureApp *) user_data;
    ClutterModifierType state = clutter_event_get_state((ClutterEvent*) event);
    bool ctrl_pressed = (state & CLUTTER_CONTROL_MASK ? true : false);
    switch (event->keyval)
    {
        case CLUTTER_KEY_q:
            if (ctrl_pressed)
                clutter_main_quit();
            break;
        case CLUTTER_KEY_space:
            //app->toggleRecord();
            break;
        case CLUTTER_KEY_0:
        case CLUTTER_KEY_1:
        case CLUTTER_KEY_2:
        case CLUTTER_KEY_3:
        case CLUTTER_KEY_4:
        case CLUTTER_KEY_5:
        case CLUTTER_KEY_6:
        case CLUTTER_KEY_7:
        case CLUTTER_KEY_8:
        case CLUTTER_KEY_9:
            {
                guint keyval = event->keyval; // clutter_event_get_key_symbol(event);
                unsigned int number_pressed = (keyval & 0x0F);
                (void) number_pressed;
                //app->chooseRecordingTrack(number_pressed);
            }
            break;
    }
}

bool AudioGestureApp::createGUI()
{
    // if (stage_ != NULL)
    // {
    //     std::cerr << "App::" << __FUNCTION__ << ": Stage already created.\n"; 
    //     return false;
    // }
    stage_ = clutter_stage_get_default();
    clutter_actor_set_size(stage_, 1024, 768);
    clutter_stage_set_color(CLUTTER_STAGE(stage_), &BLACK);
    g_signal_connect(stage_, "destroy", G_CALLBACK(clutter_main_quit), NULL);
    
    // MAIN LABEL:
    ClutterActor *main_label = clutter_text_new_full("Sans semibold 24px", "Audio gesture detector", &WHITE);
    clutter_actor_set_position(main_label, 2.0, 2.0);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage_), main_label);

    // Record button:
    learn_button_ = clutter_rectangle_new_with_color(&GRAY);
    clutter_actor_set_size(learn_button_, 50.0f, 50.0f);
    clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(learn_button_), 1);
    clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(learn_button_), &GRAY);
    clutter_actor_set_anchor_point_from_gravity(learn_button_, CLUTTER_GRAVITY_CENTER);
    clutter_actor_set_position(learn_button_, 200.0f, 150.0f);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage_), learn_button_);
    clutter_actor_set_reactive(learn_button_, TRUE);
    //g_signal_connect(learn_button_, "button-press-event", G_CALLBACK(learn_button_press_cb), this);

    ClutterActor *learn_label = clutter_text_new_full("Sans semibold 12px", "Learn", &WHITE);
    clutter_actor_set_position(learn_label, 200.0f, 200.0f);
    clutter_actor_set_anchor_point_from_gravity(learn_label, CLUTTER_GRAVITY_CENTER);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage_), learn_label);

    // Playback button:
    detect_button_ = clutter_rectangle_new_with_color(&GRAY);
    clutter_actor_set_size(detect_button_, 50.0f, 50.0f);
    clutter_rectangle_set_border_width(CLUTTER_RECTANGLE(detect_button_), 1);
    clutter_rectangle_set_border_color(CLUTTER_RECTANGLE(detect_button_), &GRAY);
    clutter_actor_set_anchor_point_from_gravity(detect_button_, CLUTTER_GRAVITY_CENTER);
    clutter_actor_set_position(detect_button_, 300.0f, 150.0f);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage_), detect_button_);
    clutter_actor_set_reactive(detect_button_, TRUE);
    //g_signal_connect(detect_button_, "button-press-event", G_CALLBACK(detect_button_press_cb), this);

    ClutterActor *detect_label = clutter_text_new_full("Sans semibold 12px", "Detect", &WHITE);
    clutter_actor_set_position(detect_label, 300.0f, 200.0f);
    clutter_actor_set_anchor_point_from_gravity(detect_label, CLUTTER_GRAVITY_CENTER);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage_), detect_label);

    detected_number_text_ = clutter_text_new_full("Sans semibold 12px", "Detected: none", &WHITE);
    clutter_actor_set_anchor_point_from_gravity(detected_number_text_, CLUTTER_GRAVITY_CENTER);
    clutter_actor_set_position(detected_number_text_, clutter_actor_get_width(stage_) / 2.0, 50.0f);
    clutter_container_add_actor(CLUTTER_CONTAINER(stage_), detected_number_text_);

    for (unsigned int i = 0; i < NUM_SPECTRAL_BANDS; i++)
    {
        std::ostringstream os;
        os << FREQ_RECTANGLE_PREFIX + i;
        ClutterActor *rect = clutter_rectangle_new_with_color(&RED);
        clutter_actor_set_size(rect, 10.0f, 10.0f);
        clutter_actor_set_name(rect, os.str().c_str());
        clutter_actor_set_position(rect, 400.0f + 30.0f * i, 400.0f);
        clutter_actor_set_anchor_point_from_gravity(rect, CLUTTER_GRAVITY_CENTER);
        clutter_container_add_actor(CLUTTER_CONTAINER(stage_), rect);
    }

    // timeline to attach a callback for each frame that is rendered
    ClutterTimeline *timeline;
    timeline = clutter_timeline_new(60); // ms
    clutter_timeline_set_loop(timeline, TRUE);
    clutter_timeline_start(timeline);

    g_signal_connect(timeline, "new-frame", G_CALLBACK(on_frame_cb), this);
    g_signal_connect(stage_, "key-press-event", G_CALLBACK(key_event_cb), this);

    clutter_actor_show(stage_);
    return true;
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

    if (clutter_init(&argc, &argv) != CLUTTER_INIT_SUCCESS)
        return 1;

    AudioGestureApp app;

    GstElement *bin;
    GstElement *src, *audioconvert, *spectrum, *sink;
    GstBus *bus;
    //GMainLoop *loop;

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
    // loop = g_main_loop_new(NULL, FALSE);
    bool success = app.launch();
    if (! success)
        std::cout << "could not launch GUI\n";
    clutter_main();
    // g_main_loop_run(loop);

    gst_element_set_state(bin, GST_STATE_NULL);
    gst_object_unref(bin);

    return 0;
}

