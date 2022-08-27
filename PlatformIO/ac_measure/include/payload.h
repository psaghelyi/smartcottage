

template <class backend_t>
struct Payload
{
    float rms;
    int vpmin, vpmax;
    int zero;
    int samples;
    int granularity;

    void reset()
    {
        rms = 0.;
        vpmin = 0;
        vpmax = 0;
        samples = 0;
        granularity = 0;
    }

    void normalize()
    {
        rms /= samples;
        vpmin /= samples;
        vpmax /= samples;
        granularity /= samples;
        zero = (vpmin + vpmax) / 2;
    }

    void upload(const backend_t &backend)
    {
        String body = "{\"voltage\": " + String(rms) +
                    ", \"peak2peak\": " + String(vpmax - vpmin) +
                    ", \"zero\": " + String(zero) +
                    ", \"samples\": " + String(samples) +
                    ", \"granularity\": " + String(granularity);

        String log = body;
        
        body += ", \"raw\": [" + String(zero) + ", ";
        for (int sample : samples)
        {
            body += String(sample) + ", ";
        }
        body += String(zero) + "]}";

        backend.upload(body);
    }
};
