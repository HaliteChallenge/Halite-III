"""Add profile image field

Revision ID: eab38b06d0a0
Revises: 1415fb3084f1
Create Date: 2018-10-03 14:22:52.903142+00:00

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql


# revision identifiers, used by Alembic.
revision = 'eab38b06d0a0'
down_revision = '1415fb3084f1'
branch_labels = None
depends_on = None


def upgrade():
    op.add_column('user',
        sa.Column('oauth_profile_image_key', sa.String(length=100), nullable=True))


def downgrade():
    op.drop_column('user', 'oauth_profile_image_key')
