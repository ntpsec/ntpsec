from waflib.TaskGen import feature, after_method


@feature("rtems_trace")
@after_method('apply_link')
def rtems_trace(self):
    if self.env.RTEMS_TEST_ENABLE:
        self.link_task.env.LINK_CC = self.env.BIN_RTEMS_TLD \
            + self.env.RTEMS_TEST_FLAGS + ['--']
