# Limits the memory of an already running Loswors process using a Windows Job Object.
# Limit = current usage + headroom (64 MB by default): the UI keeps working,
# but Argon2 (needs 256 MB) cannot allocate its memory on the next save.
#
# To remove the limit, just close Loswors (the job lives as long as the process).

param([int]$HeadroomMB = 64, [int]$ProcessId = 0)

$ErrorActionPreference = 'Stop'

Add-Type -TypeDefinition @'
using System;
using System.Runtime.InteropServices;

public static class JobLimit {
    [StructLayout(LayoutKind.Sequential)]
    struct IO_COUNTERS { public ulong R, W, O, RB, WB, OB; }

    [StructLayout(LayoutKind.Sequential)]
    struct BASIC_LIMIT {
        public long PerProcessUserTimeLimit, PerJobUserTimeLimit;
        public uint LimitFlags;
        public UIntPtr MinimumWorkingSetSize, MaximumWorkingSetSize;
        public uint ActiveProcessLimit;
        public UIntPtr Affinity;
        public uint PriorityClass, SchedulingClass;
    }

    [StructLayout(LayoutKind.Sequential)]
    struct EXTENDED_LIMIT {
        public BASIC_LIMIT Basic;
        public IO_COUNTERS Io;
        public UIntPtr ProcessMemoryLimit, JobMemoryLimit, PeakProcessMemoryUsed, PeakJobMemoryUsed;
    }

    const uint JOB_OBJECT_LIMIT_PROCESS_MEMORY = 0x100;
    const int  JobObjectExtendedLimitInformation = 9;

    [DllImport("kernel32", SetLastError = true)] static extern IntPtr CreateJobObject(IntPtr a, string n);
    [DllImport("kernel32", SetLastError = true)] static extern bool SetInformationJobObject(IntPtr j, int c, ref EXTENDED_LIMIT i, int l);
    [DllImport("kernel32", SetLastError = true)] static extern bool AssignProcessToJobObject(IntPtr j, IntPtr p);

    public static void Apply(IntPtr process, ulong limitBytes) {
        IntPtr job = CreateJobObject(IntPtr.Zero, null);
        if (job == IntPtr.Zero) throw new System.ComponentModel.Win32Exception();

        var info = new EXTENDED_LIMIT();
        info.Basic.LimitFlags = JOB_OBJECT_LIMIT_PROCESS_MEMORY;
        info.ProcessMemoryLimit = new UIntPtr(limitBytes);
        if (!SetInformationJobObject(job, JobObjectExtendedLimitInformation, ref info, Marshal.SizeOf(info)))
            throw new System.ComponentModel.Win32Exception();

        if (!AssignProcessToJobObject(job, process))
            throw new System.ComponentModel.Win32Exception();
        // The job handle is intentionally not closed: once assigned, the process
        // itself keeps the job alive, so the limit stays while Loswors is running.
    }
}
'@

$p = if ($ProcessId) { Get-Process -Id $ProcessId -ErrorAction SilentlyContinue }
     else { Get-Process -Name Loswors -ErrorAction SilentlyContinue | Select-Object -First 1 }
if (-not $p) { Write-Error 'Loswors is not running. Open your file in Loswors first, then run this script.' }

$current = [uint64]$p.PrivateMemorySize64
$limit   = $current + [uint64]$HeadroomMB * 1MB

[JobLimit]::Apply($p.Handle, $limit)

'Loswors PID {0}: current usage {1:N0} MB, limit set to {2:N0} MB (Argon2 needs 256 MB).' -f $p.Id, ($current / 1MB), ($limit / 1MB)
'Now press Save in Loswors and enter your PIN.'