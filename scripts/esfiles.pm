package esfiles;

# Strict checking
use strict;

# Standard modules
use File::Path;
use IO::Handle;
use Carp;

# Make sure all output is in order
STDOUT->autoflush(1);
STDERR->autoflush(1);

# Exported symbols
require Exporter;
our @ISA = ("Exporter");
our @EXPORT = qw(checkForChanges);

sub renameFile {
    my $from = shift;
    my $to = shift;
    rename $from, $to
      or die "Couldn't rename $from to $to: $!\n";
}

sub filesAreIdentical {
    my ($file1, $file2) = @_;
    return system("cmp -s \"$file1\" \"$file2\"") == 0;
}

sub checkForChanges {
    my $file = shift;
    my $newFile = shift;
    my $oldFile = shift;
    my $doDiff = shift;
    -e $newFile
      or confess "Internal error: no new file $newFile";
    if (!-e $file) {
        warn "Created $file\n";
        renameFile $newFile, $file;
        return;
    }
    if (filesAreIdentical $file, $newFile) {
        unlink $newFile
          or die "Couldn't remove temporary file $newFile: $!\n";
        return 0;
    } else {
        warn "Converted $file\n";
        if ((defined $doDiff) && $doDiff) {
            system("diff $file $newFile");
        }
        if (defined $oldFile) {
            unlink $oldFile;
            rename $file, $oldFile;
        } else {
            unlink $file
              or die "Couldn't remove old file $file: $!\n";
        }
        renameFile $newFile, $file;
        return 1;
    }
}
