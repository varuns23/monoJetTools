class CondorConfig():
    def __init__(self):
        self.order = list()
    def queue(self): self.order.append('Queue')
    def append(self,element): self.order.append(element)
    def __getitem__(self,index): return self.order[index]
    def __setitem__(self,key,value): self.order.append( (key,value) )
    def __iter__(self): return iter(self.order)
    def __str__(self):
        if not any(self.order): return str(self.order)
        nspace = max( len( element[0] ) for element in self if type(element) == tuple )
        spacer = '{0:<%i}' % nspace
        lines = []
        for element in self:
            if type(element) == tuple:
                key,value = element
                if type(value) == list: value = ','.join(value)
                lines.append( '%s = %s' % (spacer.format(key),value) )
            else:
                lines.append(element)
        return '\n'.join(lines)
    def write(self,fname):
        with open(fname,'w') as submit: submit.write( str(self) )
                
        

if __name__ == "__main__":

    config = CondorConfig()
    config['x509userproxy'] = '/tmp/x509up_u4549'
    config['universe'] = 'vanilla'
    config['Executable'] = 'runAnalyzer.sh'
    config['label'] = 'MET_4_0'
    config['Batch_Name'] = '$(label)'
    config['Notification'] = 'never'
    config['WhenToTransferOutput'] = 'On_Exit'
    config['ShouldTransferFiles'] = 'yes'
    config['Requirements'] = '(TARGET.UidDomain == "hep.wisc.edu" && TARGET.HAS_CMS_HDFS)'
    config['on_exit_remove'] = '(ExitBySignal == FALSE && (ExitCode == 0 || ExitCode == 42 || NumJobStarts>3))'
    config['+IsFastQueueJob'] = 'True'
    config['getenv'] = 'true'
    config['request_memory'] = 1992
    config['request_disk'] = 2048000
    config['Transfer_Input_Files'] = ['analyze']
    config['output'] = '../.status/$(label)/$(Process)_$(label).out'
    config['error']  = '../.status/$(label)/$(Process)_$(label).err'
    config['Log']    = '../.status/$(label)/$(Process)_$(label).log'

    for i in range(3):
        config['Argument'] = '/path/to/file_$(Process)'
        config.queue()
    print config
