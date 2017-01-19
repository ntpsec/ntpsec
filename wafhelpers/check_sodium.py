

def check_sodium(ctx):
    ctx.check_cc(header_name="sodium.h", mandatory=True,
                 errmsg="No\nFatal Error: Your system is missing libsodium")
    ctx.check_cc(lib="sodium", comment="Sodium crypto library", mandatory=True)
